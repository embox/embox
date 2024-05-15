/**
 * @file
 * @brief
 *
 * @author  Vadim Deryabkin
 * @date    27.01.2020
 */

#include <bsp/stm32cube_hal.h>
#include <hal/platform.h>

void platform_shutdown(shutdown_mode_t mode) {
	switch (mode) {
	case SHUTDOWN_MODE_HALT:
	case SHUTDOWN_MODE_REBOOT:
	case SHUTDOWN_MODE_ABORT:
	default:
		HAL_NVIC_SystemReset();
		break;
	}

	/* NOTREACHED */
	while (1) {}
}
