/**
 * @file
 * @brief Implements ARCH interface for sparc processors
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <hal/arch.h>
#include <asm/cache.h>
#include <hal/ipl.h>

void arch_init(void) {
	cache_enable();
}

void arch_idle(void) {
}

void __attribute__ ((noreturn)) arch_shutdown(arch_shutdown_mode_t mode) {
	ipl_disable();
	// TODO DUMP regs and memory -- Eldar
	while (1) {}
}

