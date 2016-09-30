/**
 * @file
 * @brief Implements ARCH interface for sparc processors
 *
 * @date 14.02.10
 * @author Eldar Abusalimov
 */

#include <hal/arch.h>
#include <asm/cache.h>
#include <hal/ipl.h>
#include <compiler.h>

void arch_init(void) {
	cache_enable();
}

void arch_idle(void) {
	__asm__ __volatile__ ("wr %g0, %asr19");
}

unsigned int arch_excep_disable(void) {
	unsigned int ret;
	unsigned int tmp;
	__asm__ __volatile__ (
		"rd %%psr, %0\n\t"
		"andn %0, %2, %1\n\t"
		"wr %1, 0, %%psr\n\t"
		" nop; nop; nop\n"
		: "=&r" (ret), "=r" (tmp)
		: "i" (PSR_ET)
		: "memory"
	);
	return ret;
}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {

	ipl_disable();
	arch_excep_disable();
	asm ("ta 0");

	while (1) {}
}
