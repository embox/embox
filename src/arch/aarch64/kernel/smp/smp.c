/**
 * @file
 * @brief Starting and stopping the secondary cores (aarch64).
 *
 * A core arrives with the MMU and the caches off, which decides the shape of
 * the boot path: it programs its own TTBR/TCR/MAIR before it can use the
 * primary's tables, it may not take a lock or an atomic until then (an
 * exclusive needs Normal cacheable memory), and everything it reads on the
 * way in is cleaned to the point of coherency by the primary first.
 */

#include <stdint.h>

#include <drivers/irqctrl.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/cache.h>
#include <hal/cpu.h>
#include <hal/cpu_idle.h>
#include <hal/ipl.h>
#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <kernel/cpu/cpu.h>
#include <kernel/cpu/cpudata.h>
#include <kernel/critical.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/sched.h>
#include <kernel/spinlock.h>
#include <kernel/task.h>
#include <kernel/printk.h>
#include <kernel/task/kernel_task.h>
#if OPTION_GET(NUMBER, spintable_base) != 0
#include <sys/mman.h>

#include <hal/mmu.h>
#include <mem/vmem.h>
#include <mem/vmem_device_memory.h>
#endif
#include <kernel/thread.h>
#include <kernel/time/clock_source.h>
#include <util/log.h>

#include <module/embox/kernel/thread/core.h>

#include <aarch64/cpu_id.h>
#include <aarch64/psci.h>
#include <aarch64/smp.h>
#include <util/atomic_rmw.h>

#define THREAD_STACK_SIZE \
	OPTION_MODULE_GET(embox__kernel__thread__core, NUMBER, thread_stack_size)

/* 0 on a board whose cores are started with PSCI */
#define SPINTABLE_BASE OPTION_GET(NUMBER, spintable_base)

EMBOX_UNIT_INIT(aarch64_smp_init);

/**
 * What one secondary needs before it has an MMU, read with the caches off.
 *
 * `sp` is first because the reset handler loads it from offset 0, in
 * assembly. `claim` is cleared when the primary gives up waiting, and a core
 * that arrives after that parks instead of joining. One entry per core, a
 * cache line each, so that cleaning one entry does not disturb another.
 */
struct aarch64_ap_boot {
	uint64_t sp;
	uint64_t tcr;
	uint64_t mair;
	uint64_t ttbr0;
	uint64_t ttbr1;
	volatile uint64_t ack;
	volatile uint64_t claim;
	uint64_t pad;
};

_Static_assert(sizeof(struct aarch64_ap_boot) == 64,
    "reset_handler.S indexes aarch64_ap_boot with lsl #6");

struct aarch64_ap_boot aarch64_ap_boot[NCPU] __attribute__((aligned(64)));

/* The entry point PSCI is told to start a core at */
extern char aarch64_ap_entry;

/* The entry point of a core released off a spin table, which arrives with
 * its registers zeroed and reads its own id out of MPIDR */
extern char aarch64_ap_entry_mpidr;

/* One stack per secondary. Its idle thread is initialised on it, so it may
 * not be smaller than thread_stack_size. */
static char ap_stack[NCPU - 1][KERNEL_AP_STACK_SZ] __attribute__((aligned(16)));

/* Secondaries come up one at a time */
static spinlock_t startup_lock = SPIN_STATIC_UNLOCKED;

static void *ap_idle_run(void *arg) {
	(void)arg;
	panic("aarch64 smp: the idle thread of a secondary returned\n");
}

extern void thread_set_current(struct thread *t);

void aarch64_startup_ap(unsigned int cpu_id);

/* Reschedule IPIs taken, each CPU counting in its own copy */
static unsigned long ipi_count __cpudata__;

unsigned long aarch64_smp_ipi_count(unsigned int cpu_id) {
	if (cpu_id >= NCPU) {
		return 0;
	}
	/* An aligned word load is single-copy atomic here; volatile is what
	 * keeps a polling reader from holding the value in a register */
	return *(volatile unsigned long *)cpudata_cpu_ptr(cpu_id, &ipi_count);
}

/**
 * The other end of smp_send_resched(): the SGI number is the whole message.
 *
 * Noyield, because this core was asked to look at the run queue, not to give
 * up the slice of whatever is running on it.
 */
static irq_return_t resched_ipi_handler(unsigned int irq_nr, void *data) {
	extern void sched_post_switch_noyield(void);
	volatile unsigned long *count;

	(void)irq_nr;
	(void)data;

	count = cpudata_ptr(&ipi_count);
	*count += 1;

	sched_post_switch_noyield();

	return IRQ_HANDLED;
}

/**
 * Wake a CPU up to reschedule. Called from interrupt handlers in sched.c and
 * sched_ticker.c; a CPU that is not up yet is dropped by the driver.
 */
void smp_send_resched(int cpu_id) {
	irqctrl_send_ipi((unsigned int)cpu_id, AARCH64_SGI_RESCHED);
}

/**
 * Start this core's own tick: the timers a thread queues while running here
 * go on this core's list, and only this core walks it.
 *
 * The timer registers and its PPI are per core, so set_periodic() and
 * irqctrl_enable() speak for the caller; the handler is attached once.
 */
static void ap_clock_start(void) {
	extern const struct clock_source *cs_jiffies;
	const struct time_event_device *ed;

	if (!cs_jiffies || !cs_jiffies->event_device) {
		log_warning("aarch64 smp: no clock source, cpu %u will not tick",
		    cpu_get_id());
		return;
	}

	ed = cs_jiffies->event_device;
	if (!ed->set_periodic) {
		log_warning("aarch64 smp: clock source %s has no periodic mode",
		    ed->name);
		return;
	}

	irqctrl_enable(ed->irq_nr);
	ed->set_periodic((struct clock_source *)cs_jiffies);
}

/* Stopping the other cores when one of them aborts, and what they report */

struct aarch64_cpu_report {
	volatile int state; /* 0 not asked, 1 asked, 2 answered */
	unsigned long pc;
	unsigned long lr;
	unsigned long sp;
	unsigned long psr;
	unsigned long crit;
	void *schedee;
	int is_idle;
};

static struct aarch64_cpu_report cpu_report[NCPU];
static volatile int smp_stopping;
static volatile int smp_printed;

/**
 * Record where this core was and park it, interrupts masked. Reached from the
 * interrupt entry before any lock is taken, and never returns.
 */
void aarch64_smp_stop_self(unsigned long pc, unsigned long lr, unsigned long sp,
    unsigned long psr) {
	struct aarch64_cpu_report *r;
	struct schedee *cur;
	struct thread *idle;
	unsigned int me;

	ipl_disable();

	me = cpu_get_id();
	r = &cpu_report[me];

	r->pc = pc;
	r->lr = lr;
	r->sp = sp;
	r->psr = psr;
	r->crit = (unsigned long)critical_count();

	cur = schedee_get_current();
	idle = cpu_get_idle(me);
	r->schedee = cur;
	r->is_idle = (cur && idle && cur == &idle->schedee);

	/* Release: the report has to be visible before the state that claims it */
	atomic_rmw_store(&r->state, 2, __ATOMIC_RELEASE);

	while (1) {
		arch_cpu_idle();
	}
}

void smp_stop_others(void) {
	unsigned int self, i;
	unsigned long spins;
	int asked = 0;

	/* Two cores panicking at once must not wait for each other. */
	if (atomic_rmw_exchange(&smp_stopping, 1, __ATOMIC_ACQ_REL)) {
		return;
	}

	self = cpu_get_id();

	for (i = 0; i < NCPU; i++) {
		if (i == self || cpu_get_idle(i) == NULL) {
			continue;
		}
		cpu_report[i].state = 1;
		irqctrl_send_ipi(i, AARCH64_SGI_STOP);
		asked++;
	}

	if (!asked) {
		return;
	}

	/* Bounded: a core with interrupts masked will never answer */
	for (spins = 0; spins < 200000000UL; spins++) {
		int pending = 0;

		for (i = 0; i < NCPU; i++) {
			if (cpu_report[i].state == 1) {
				pending = 1;
			}
		}
		if (!pending) {
			break;
		}
		__barrier();
	}
	__atomic_thread_fence(__ATOMIC_ACQUIRE);
}

/**
 * Print what the stopped cores reported. Separate from stopping them: they
 * are stopped before anything is printed, and reported after the failure.
 */
void smp_print_stopped(void) {
	unsigned int self, i;

	if (!smp_stopping || atomic_rmw_exchange(&smp_printed, 1, __ATOMIC_ACQ_REL)) {
		return;
	}

	self = cpu_get_id();

	printk("\n== other cpus at the time of the abort ==\n");
	for (i = 0; i < NCPU; i++) {
		struct aarch64_cpu_report *r = &cpu_report[i];

		if (i == self) {
			printk("  cpu%u  <- the one that aborted\n", i);
			continue;
		}
		if (cpu_get_idle(i) == NULL) {
			printk("  cpu%u  never started\n", i);
			continue;
		}
		if (r->state != 2) {
			printk("  cpu%u  did not answer (interrupts masked, or wedged)\n",
			    i);
			continue;
		}
		printk("  cpu%u  pc %#018lx  lr %#018lx\n", i, r->pc, r->lr);
		/* The raw schedee pointer: two cores naming the same one is the
		 * case worth seeing */
		printk("        sp %#018lx  psr %#010lx  crit %#lx  schedee %p  %s\n",
		    r->sp, r->psr, r->crit, r->schedee,
		    r->is_idle ? "(idle)" : "(running)");
	}
	printk("\n");
}

/**
 * The first C a secondary runs, with the MMU still off: it takes over the
 * primary's translation registers and turns translation on.
 *
 * Nothing here may take a lock or call into the kernel proper -- both need an
 * exclusive, and memory is neither cacheable nor shareable until the MMU is on.
 */
void aarch64_ap_start(unsigned int cpu_id) {
	struct aarch64_ap_boot *slot = &aarch64_ap_boot[cpu_id];

	ARCH_REG_STORE(TCR_EL1, slot->tcr);
	ARCH_REG_STORE(MAIR_EL1, slot->mair);
	ARCH_REG_STORE(TTBR0_EL1, slot->ttbr0);
	ARCH_REG_STORE(TTBR1_EL1, slot->ttbr1);
	isb();

	__asm__ __volatile__("tlbi vmalle1" : : : "memory");
	__asm__ __volatile__("ic iallu" : : : "memory");
	dsb(sy);
	isb();

	ARCH_REG_ORIN(SCTLR_EL1, SCTLR_ELn_M | SCTLR_ELn_C | SCTLR_ELn_I);
	isb();

	aarch64_startup_ap(cpu_id);
}

void aarch64_startup_ap(unsigned int cpu_id) {
	struct thread *idle;

	/* Park if the primary gave up on this core: the system has already
	 * counted itself without it */
	if (!atomic_rmw_load(&aarch64_ap_boot[cpu_id].claim, __ATOMIC_ACQUIRE)) {
		while (1) {
			arch_cpu_idle();
		}
	}

	__spin_lock(&startup_lock);

	/* Record this core's MPIDR; TPIDR_EL1 was set by the reset handler */
	aarch64_cpu_id_register(cpu_id);

	/* This core's own half of the controller, then what it listens to */
	irqctrl_init_cpu();
	irqctrl_enable(AARCH64_SGI_RESCHED);
	irqctrl_enable(AARCH64_SGI_STOP);
	ap_clock_start();

	idle = thread_init_stack((char *)(uintptr_t)aarch64_ap_boot[cpu_id].sp
	                             - THREAD_STACK_SIZE,
	    THREAD_STACK_SIZE, SCHED_PRIORITY_MIN, ap_idle_run, NULL);
	cpu_init(cpu_id, idle);
	task_thread_register(task_kernel_task(), idle);
	thread_set_current(idle);
	sched_set_current(&idle->schedee);

	log_info("aarch64 smp: cpu %u up, MPIDR %#010llx, EL%llu", cpu_id,
	    (unsigned long long)aarch64_cpu_mpidr(cpu_id),
	    (unsigned long long)((ARCH_REG_LOAD(CurrentEL) >> 2) & 0x3));

	aarch64_ap_boot[cpu_id].ack = 1;
	__spin_unlock(&startup_lock);

	ipl_enable();

	while (1) {
		arch_cpu_idle();
	}
}

/**
 * The assumed MPIDR of a core that is not running yet: Aff0 = @a cpu_id in
 * the primary's cluster. A multi-cluster board would have to read its tree.
 */
static uint64_t ap_mpidr(unsigned int cpu_id) {
	return (aarch64_cpu_mpidr(0) & ~0xffULL) | cpu_id;
}

/**
 * Release a core the firmware parked on a spin table, the way a Raspberry Pi
 * and QEMU's raspi model do: it polls its word with the caches off, hence the
 * flush, and sleeps on WFE between reads, hence the SEV.
 */
static void cpu_release_spintable(unsigned int cpu_id) {
	volatile uint64_t *slot;

	slot = (volatile uint64_t *)(uintptr_t)(SPINTABLE_BASE + 8 * cpu_id);
	*slot = (uint64_t)(uintptr_t)&aarch64_ap_entry_mpidr;
	dcache_flush((void *)slot, sizeof(*slot));
	dsb(sy);
	__asm__ __volatile__("sev" ::: "memory");
}

#if SPINTABLE_BASE != 0
/* The release words sit in the first page of RAM, which nothing maps. It is
 * mapped around the release only, so that a null dereference stays a fault
 * rather than a silent write into the spin table. */
static uintptr_t spintable_page(void) {
	return (uintptr_t)SPINTABLE_BASE & ~(uintptr_t)MMU_PAGE_MASK;
}

static void spintable_map(void) {
	mmap_device_memory((void *)spintable_page(), MMU_PAGE_SIZE,
	    PROT_READ | PROT_WRITE, MAP_FIXED, spintable_page());
}

static void spintable_unmap(void) {
	vmem_unmap_region(vmem_current_context(), spintable_page(),
	    MMU_PAGE_SIZE);
	mmu_flush_tlb();
}
#endif /* SPINTABLE_BASE */

static int cpu_start(unsigned int cpu_id) {
	unsigned long spins;
	long ret;

	struct aarch64_ap_boot *slot = &aarch64_ap_boot[cpu_id];

	slot->sp = (uint64_t)(uintptr_t)&ap_stack[cpu_id - 1][KERNEL_AP_STACK_SZ];
	slot->ack = 0;
	slot->claim = 1;

	/* The core about to read this has no caches on */
	dcache_flush(slot, sizeof(*slot));
	dsb(sy);

	if (SPINTABLE_BASE != 0) {
		cpu_release_spintable(cpu_id);
	}
	else {
		ret = psci_cpu_on(ap_mpidr(cpu_id), (uintptr_t)&aarch64_ap_entry,
		    cpu_id);
		if (ret != PSCI_SUCCESS) {
			log_error("aarch64 smp: CPU_ON(cpu %u, MPIDR %#010llx) = %ld",
			    cpu_id, (unsigned long long)ap_mpidr(cpu_id), ret);
			return -1;
		}
	}

	/* Bounded, so that a core which never arrives costs the boot a delay
	 * and a log line rather than a hang */
	for (spins = 0; spins < 1000000000UL; spins++) {
		if (slot->ack) {
			/* The ack is a plain store on the other side: acquire what
			 * that core published before it -- its idle thread, its frame */
			__atomic_thread_fence(__ATOMIC_ACQUIRE);
			return 0;
		}
		__barrier();
	}

	/* Revoke the entry: a core that is merely late reads this and parks */
	atomic_rmw_store(&slot->claim, 0, __ATOMIC_RELEASE);
	dcache_flush(slot, sizeof(*slot));
	dsb(sy);

	log_error("aarch64 smp: cpu %u did not answer after %s; revoked, and it"
	          " will park if it arrives later",
	    cpu_id, SPINTABLE_BASE ? "the spin-table release" : "CPU_ON");
	return -1;
}

static int aarch64_smp_init(void) {
	unsigned int self, i;
	int up = 1;
	int ret;

	aarch64_cpu_id_register(0);

	/* Attaches the timer_sharing that sched_ticker_update() dereferences
	 * under SMP, as the x86 and RISC-V ports do from their own unit init */
	sched_ticker_set_shared();

	self = cpu_get_id();

	/* The stop IPI has no handler -- the interrupt entry serves it -- but it
	 * still has to be delivered, and an SGI is enabled per core */
	irqctrl_enable(AARCH64_SGI_STOP);

	/* What a secondary needs to reach the same address space */
	for (i = 1; i < NCPU; i++) {
		aarch64_ap_boot[i].tcr = ARCH_REG_LOAD(TCR_EL1);
		aarch64_ap_boot[i].mair = ARCH_REG_LOAD(MAIR_EL1);
		aarch64_ap_boot[i].ttbr0 = ARCH_REG_LOAD(TTBR0_EL1);
		aarch64_ap_boot[i].ttbr1 = ARCH_REG_LOAD(TTBR1_EL1);
	}

	if (NCPU == 1) {
		log_info("aarch64 smp: single cpu, MPIDR %#010llx",
		    (unsigned long long)aarch64_cpu_mpidr(self));
		return 0;
	}

	if (!psci_available() && SPINTABLE_BASE == 0) {
		log_warning("aarch64 smp: no PSCI conduit and no spin table, %d of %d "
		            "cpus stay down",
		    (int)NCPU - 1, (int)NCPU);
		return 0;
	}

	/* Once, before any secondary exists: the handler table is global and a
	 * core that comes up only enables the line */
	ret = irq_attach(AARCH64_SGI_RESCHED, resched_ipi_handler, 0, NULL,
	    "smp resched");
	if (ret != 0) {
		log_error("aarch64 smp: cannot attach SGI %d: %d", AARCH64_SGI_RESCHED,
		    ret);
		return 0;
	}

	/* Say which mechanism this image starts its cores with */
	if (SPINTABLE_BASE != 0) {
		log_info("aarch64 smp: cpu %u up, MPIDR %#010llx, spin table at %#x",
		    self, (unsigned long long)aarch64_cpu_mpidr(self),
		    (unsigned)SPINTABLE_BASE);
	}
	else {
		log_info("aarch64 smp: cpu %u up, MPIDR %#010llx, PSCI %s version %#lx",
		    self, (unsigned long long)aarch64_cpu_mpidr(self), psci_method(),
		    psci_version());
	}

#if SPINTABLE_BASE != 0
	spintable_map();
#endif

	for (i = 0; i < NCPU; i++) {
		if (i == self) {
			continue;
		}
		if (cpu_start(i) == 0) {
			up++;
		}
	}

#if SPINTABLE_BASE != 0
	/* cpu_start() has waited for every core that was going to answer */
	spintable_unmap();
#endif

	log_info("aarch64 smp: %d of %d cpus up", up, (int)NCPU);

	return 0;
}
