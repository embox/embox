/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.03.2013
 */

#include <hal/arch.h>
#include <kernel/host.h>

void arch_init(void) {
}

void arch_idle(void) {
	host_pause();
}

void arch_shutdown(arch_shutdown_mode_t mode) {
	while (1);
}

