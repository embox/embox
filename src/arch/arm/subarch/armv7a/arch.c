/**
 * @file
 * @brief
 *
 * @date 25.11.22
 * @author Aleksey Zhmulin
 */
#include <hal/arch.h>

void arch_init(void) {
}

void arch_idle(void) {
	__asm__ __volatile__("WFI");
}

void arch_shutdown(arch_shutdown_mode_t mode) {
	while (1)
		;
}
