/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.08.2014
 */

#include <hal/reg.h>
#include <platform_shutdown.h>

void platform_shutdown(arch_shutdown_mode_t mode) {
	if (mode == ARCH_SHUTDOWN_MODE_REBOOT) {
		REG_STORE(0x481800a0, 0x2);
	}
}
