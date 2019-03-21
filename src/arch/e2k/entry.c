#include <string.h>

#include <asm/io.h>
#include <e2k_api.h>
#include <asm/ptrace.h>
#include <hal/ipl.h>
#include <framework/mod/options.h>

#define CPU_COUNT OPTION_GET(NUMBER, cpu_count)

#define E2K_DONE \
do { \
    asm volatile ("{nop 3} {done}" ::: "ctpr3"); \
} while (0)

static int volatile entries_count = 0;
static int volatile idled_cpus_count = 0;

static void e2k_kernel_start(void) {
	extern void kernel_start(void);
	int psr;

	/* Wait until all another CPUs will reach idle state,
	 * before exceptions enabling. */
	while (idled_cpus_count < CPU_COUNT - 1)
		;

	/* PSR is local register and makes sence only within a function,
	* so we set it here before kernel start. */
	asm volatile ("rrs %%psr, %0" : "=r"(psr) :);
	psr |= (PSR_IE | PSR_NMIE | PSR_UIE);
	asm volatile ("rws %0, %%psr" : : "ri"(psr));

	/* XXX Disable FPU. Should be enabled later. */
	e2k_upsr_write(e2k_upsr_read() & ~UPSR_FE);

	kernel_start();
}

__attribute__ ((__section__(".e2k_entry")))
void e2k_entry(struct pt_regs *regs) {
	/* Since we enable exceptions only when all CPUs except the main one
	 * reached the idle state (cpu_idle), we can rely that order and can
	 * guarantee exceptions happen strictly after all CPUS entries. */
	if (entries_count >= CPU_COUNT) {
		/* Entering here because of expection or interrupt */
		e2k_trap_handler(regs);

		RESTORE_COMMON_REGS(regs);

		E2K_DONE;
	}

	/* It wasn't expection, so we decide this usual program execution,
	 * that is, Embox started on CPU0 or CPU1 */

	e2k_wait_all();

	entries_count = __e2k_atomic32_add(1, &entries_count);

	if (entries_count > 1) {
		/* XXX currently we support only single core */
		cpu_idle();
	}

	e2k_kernel_start();
}

__attribute__ ((__section__(".cpu_idle")))
void cpu_idle(void) {
	idled_cpus_count = __e2k_atomic32_add(1, &idled_cpus_count);
	while(1)
		;
}
