/**
 * @file
 * @brief
 *
 * @date 25.11.22
 * @author Aleksey Zhmulin
 */
#include <stdint.h>

#include <arm/fpu.h>
#include <hal/arch.h>

void arch_init(void) {
#ifdef ARM_FPU_VFP
	uint32_t val;

	/* Allow access to c10 & c11 coprocessors */
	asm volatile("mrc p15, 0, %0, c1, c0, 2" : "=r"(val) :);
	val |= 0xf << 20;
	asm volatile("mcr p15, 0, %0, c1, c0, 2" : : "r"(val));
#endif
}

void arch_idle(void) {
	__asm__ __volatile__("WFI");
}

void arch_shutdown(arch_shutdown_mode_t mode) {
	while (1)
		;
}
