#include <util/log.h>

#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include <asm/io.h>
#include <asm/mpspec.h>
#include <asm/atomic_api.h>
#include <asm/cpu_regs_types.h>
#include <asm/ptrace.h>
#include <e2k_api.h>

#include <hal/ipl.h>

#include <framework/mod/options.h>
#include <drivers/interrupt/lapic/regs.h>

extern void e2k_trap_handler(struct pt_regs *regs);

#define CPU_COUNT OPTION_GET(NUMBER, cpu_count)


static int volatile entries_count = 0;
static int volatile idled_cpus_count = 0;
static int volatile sync_count = 0;

static void e2k_kernel_start(void) {
	extern void kernel_start(void);
	int psr;

	/* Wait until all another CPUs will reach idle state,
	 * before exceptions enabling. */

	log_debug("*idled_cpus_count=0x%x\n",idled_cpus_count);
	log_debug("*entries_count=0x%x\n",entries_count);
	log_debug("*sync_count=0x%x\n",sync_count);
	while (idled_cpus_count < CPU_COUNT - 1)
		;

	/* PSR is local register and makes sense only within a function,
	* so we set it here before kernel start. */
	asm volatile ("rrs %%psr, %0" : "=r"(psr) :);
	psr |= (PSR_IE | PSR_NMIE | PSR_UIE);
	asm volatile ("rws %0, %%psr" : : "ri"(psr));

	/* XXX Disable FPU. Should be enabled later. */
	e2k_upsr_write(e2k_upsr_read() & ~UPSR_FE);

	entries_count = 8;

	mpspec_init();

	kernel_start();
}

__attribute__ ((__section__(".cpu_idle")))
void cpu_idle(void) {
	__api_atomic32_add_oldval_lock(1, &idled_cpus_count);
	while(1)
		;
}

#include <hal/context.h>
#include <hal/cpu.h>
static struct context cpu_ctx_prev[2];
static struct context cpu_ctx[2];/* 8 CPU */
static char idle_stack1[0x100000] __attribute__((aligned(0x4000)));
#include <module/embox/kernel/stack.h>

extern char _stack_top;

__attribute__ ((__section__(".e2k_entry")))
void e2k_entry(struct pt_regs *regs) {
	/* Since we enable exceptions only when all CPUs except the main one
	 * reached the idle state (cpu_idle), we can rely that order and can
	 * guarantee exceptions happen strictly after all CPUS entries. */
	if (entries_count >= CPU_COUNT) {
		/* Entering here because of exception or interrupt */
		e2k_trap_handler(regs);

		RESTORE_COMMON_REGS(regs);
#if 0
		E2K_SET_DSREG(tir.lo, 0); /* Copy from boot tt */
		E2K_SET_DSREG(tir.hi, 0);
#endif
		E2K_DONE;
	}

	/* It wasn't exception, so we decide this usual program execution,
	 * that is, Embox started on CPU0 or CPU1 */
	E2K_WAIT_ALL;

	__api_atomic32_add_oldval_lock(1, &entries_count);


	if (lapic_read(0xFEE00020)>>24 != 0x0) { // all non-BSP core go to sleep
		/* XXX currently we support only single core */
		/* Run cpu_idle on 2nd CPU */

		/* it's just needed if log_debug enabled in e2k_context module
		 * else output will be wrong because 2 cpu printing at the same time */
		while(!sync_count);

		context_init(&cpu_ctx[0], CONTEXT_PRIVELEGED | CONTEXT_IRQDISABLE,
				cpu_idle, idle_stack1, sizeof(idle_stack1));
		context_switch(&cpu_ctx_prev[0], &cpu_ctx[0]);
	}


	/* Start kernel on BSP processor */
	if (lapic_read(0xFEE00020)>>24 == 0x0) {

		/* clean .bss */
		extern int _bss_vma, _bss_len;
		extern int _data_vma, _data_lma, _data_len;
		memset(&_bss_vma, 0, (size_t) &_bss_len);

		if (&_data_vma != &_data_lma) {
			memcpy(&_data_vma, &_data_lma, (size_t) &_data_len);
		}

		context_init(&cpu_ctx[1], CONTEXT_PRIVELEGED | CONTEXT_IRQDISABLE,
				e2k_kernel_start, &_stack_top, KERNEL_BSP_STACK_SZ);
		__api_atomic32_add_oldval_lock(1, &sync_count);
		context_switch(&cpu_ctx_prev[1], &cpu_ctx[1]);
	}
}
