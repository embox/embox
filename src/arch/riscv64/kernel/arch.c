/**
 * @file
 *
 * @date June 17, 2020
 * @author: sksat
 */

#include <hal/arch.h>
#include <compiler.h>

void arch_init(void) {
}

void arch_idle(void) {
}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {}
}
