/**
 * @file
 * @brief
 *
 * @date 21.06.10
 * @author Anton Kozlov
 */

#include <hal/arch.h>
#include <platform_shutdown.h>

void arch_init(void) {

}

void arch_idle(void) {
}

void arch_shutdown(arch_shutdown_mode_t mode) {
	platform_shutdown(mode);
	while (1);
}
