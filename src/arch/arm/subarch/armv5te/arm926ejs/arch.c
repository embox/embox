/*
 * @file
 *
 * @date 24.05.2020
 * @author sksat
 */

#include <hal/arch.h>

void arch_init(void) {
}

void arch_idle(void) {
	// DBGITCTRL
	__asm__ __volatile__(
			"mov r0, #0\n"
			"mcr p15, 0, r0, c7, c0, 4");
}

void arch_shutdown(arch_shutdown_mode_t mode) {
	while(1);
}
