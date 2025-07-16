/**
 * @file
 * @brief
 *
 * @date 14.02.10
 * @author Eldar Abusalimov
 */

#include <compiler.h>

#include <asm/psr.h>
#include <hal/cpu_idle.h>
#include <hal/ipl.h>
#include <hal/platform.h>

static unsigned int arch_excep_disable(void) {
	unsigned int ret;
	unsigned int tmp;
	__asm__ __volatile__("rd %%psr, %0\n\t"
	                     "andn %0, %2, %1\n\t"
	                     "wr %1, 0, %%psr\n\t"
	                     " nop; nop; nop\n"
	                     : "=&r"(ret), "=r"(tmp)
	                     : "i"(PSR_ET)
	                     : "memory");
	return ret;
}

void _NORETURN platform_shutdown(shutdown_mode_t mode) {
	ipl_disable();
	arch_excep_disable();

	asm("ta 0");

	while (1) {
		arch_cpu_idle();
	}
}
