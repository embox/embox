/**
 * @file
 * @brief
 *
 * @date 24.12.2012
 * @author Anton Bulychev
 */

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <asm/ap.h>
#include <asm/msr.h>
#include <drivers/common/memory.h>
#include <embox/unit.h>
#include <hal/cpu_idle.h>
#include <kernel/cpu/cpu.h>
#include <kernel/panic.h>
#include <kernel/spinlock.h>
#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/thread.h>

#include <module/embox/driver/interrupt/lapic.h>
#include <module/embox/kernel/thread/core.h>

#define THREAD_STACK_SIZE \
	OPTION_MODULE_GET(embox__kernel__thread__core, NUMBER, thread_stack_size)

EMBOX_UNIT_INIT(unit_init);
#define TRAMPOLINE_ADDR 0x20000UL
extern void idt_load(void);

static char ap_stack[NCPU - 1][KERNEL_AP_STACK_SZ]
    __attribute__((aligned(KERNEL_AP_STACK_SZ)));
static int ap_ack;
static spinlock_t startup_lock = SPIN_STATIC_UNLOCKED;

static void *bs_idle_run(void *arg) {
	panic("%s runned\n", __func__);
}

extern void thread_set_current(struct thread *t);

void startup_ap(void) {
	struct thread *bs_idle;
	int self_id = cpu_get_id();

	__spin_lock(&startup_lock);

	idt_load();
	apic_init();
	irqctrl_enable(2);

	bs_idle = thread_init_stack(__ap_sp[self_id - 1] - THREAD_STACK_SIZE, THREAD_STACK_SIZE,
	    SCHED_PRIORITY_MIN, bs_idle_run, NULL);
	cpu_init(self_id, bs_idle);
	task_thread_register(task_kernel_task(), bs_idle);
	thread_set_current(bs_idle);
	sched_set_current(&bs_idle->schedee);

	ap_ack = 1;

	__spin_unlock(&startup_lock);
	ipl_enable();

	while (1) {
		arch_cpu_idle();
	}
}

static inline void init_trampoline(void) {
	/* Initialize gdt */
	memcpy(__ap_gdt, gdt, sizeof(gdt));
	gdt_set_gdtr(&__ap_gdtr, __ap_gdt);

	memcpy((void *)TRAMPOLINE_ADDR, &__ap_trampoline,
	    (uint32_t)&__ap_trampoline_end - (uint32_t)&__ap_trampoline);
}

/* TODO: FIX THIS! */
static inline void cpu_start(int cpu_id) {
	/* Setting up stack and boot */
	__ap_sp[cpu_id - 1] = ap_stack[cpu_id] + KERNEL_AP_STACK_SZ;

	lapic_send_init_ipi(cpu_id);

	ap_ack = 0;

	lapic_send_startup_ipi(cpu_id, TRAMPOLINE_ADDR);
	
	while (!ap_ack)
		__barrier();
}

static int unit_init(void) {
	int i, self_id;

	/* Initialize trampoline for the APs */
	init_trampoline();

	/* Start all CPUs */
	self_id = cpu_get_id();
	for (i = 0; i < NCPU; i++) {
		if (i != self_id)
			cpu_start(i);
	}

	return 0;
}

void smp_send_resched(int cpu_id) {
	lapic_send_ipi(0x50, cpu_id, LAPIC_ICR_DEST_FIELD);
}

void resched(void) {
	extern void sched_post_switch(void);
	extern void sched_post_switch_noyield(void);

	lapic_send_eoi();

	//sched_post_switch();
	sched_post_switch_noyield();
}

/* FIXME this is a code (not device memory) region */
PERIPH_MEMORY_DEFINE(smp_trampoline, TRAMPOLINE_ADDR, 0x1000);
