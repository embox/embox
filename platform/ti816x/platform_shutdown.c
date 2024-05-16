/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.08.2014
 */

#include <hal/platform.h>
#include <hal/reg.h>

void platform_shutdown(shutdown_mode_t mode) {
	if (mode == SHUTDOWN_MODE_REBOOT) {
		REG_STORE(0x481800a0, 0x2);
	}
	while (1) {}
}
