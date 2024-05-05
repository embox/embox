/**
 * @brief
 *
 * @date 20.12.23
 * @author Aleksey Zhmulin
 */
#include <compiler.h>

#include <hal/cpu_idle.h>
#include <hal/platform.h>

void _NORETURN platform_shutdown(shutdown_mode_t mode) {
	while (1) {
		arch_cpu_idle();
	}
}
