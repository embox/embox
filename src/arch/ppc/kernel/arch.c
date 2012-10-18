/**
 * @file
 *
 * @date Oct 17, 2012
 * @author: Anton Bondarev
 */

#include <hal/arch.h>

void arch_init(void) {
}

void arch_idle(void) {
}

void __attribute__ ((noreturn)) arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {}
}
