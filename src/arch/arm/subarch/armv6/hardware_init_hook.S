/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 14.05.24
 */

#include <arm/fpu.h>

.section .text

.global hardware_init_hook

hardware_init_hook:
#ifdef ARM_FPU_VFP
	/* Allow access to c10 & c11 coprocessors */
	mrc     p15, 0, r0, c1, c0, 2
	orr     r0, r0, #(0xf << 20)
	mcr     p15, 0, r0, c1, c0, 2
#endif

	/* Return to bootloader */
	mov     pc, lr
