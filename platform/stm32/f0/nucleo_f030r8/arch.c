/**
 * @file
 * @brief
 *
 * @author  Vadim Deryabkin
 * @date    27.01.2020
 */

#include <stdint.h>
#include <hal/reg.h>
#include <hal/arch.h>

void arch_init() {
}

void arch_idle() {
}

void arch_shutdown(arch_shutdown_mode_t mode) {
	switch (mode) {
	case ARCH_SHUTDOWN_MODE_HALT:
	case ARCH_SHUTDOWN_MODE_REBOOT:
	case ARCH_SHUTDOWN_MODE_ABORT:
	default:
		break;
	}

	/* NOTREACHED */
	while(1) {
	}
}
