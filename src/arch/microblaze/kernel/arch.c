/**
 * @file
 * @brief TODO stub
 *
 * @date 14.02.10
 * @author Anton Bondarev
 */

#include <hal/arch.h>
#include <asm/cache.h>
#include <compiler.h>

void arch_init(void) {
	cache_enable();
}

void arch_idle(void) {
}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {}
}
