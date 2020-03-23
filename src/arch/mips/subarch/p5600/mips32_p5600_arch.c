/**
 * @file
 *
 * @date Mar 20, 2020
 * @uthor Anton Bondarev
 */

#include <util/log.h>

#include <stdint.h>

#include <asm/mipsregs.h>

#include <hal/arch.h>

void arch_init(void) {
}

void arch_idle(void) {

}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {}
}
