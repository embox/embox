/**
 * @file
 *
 * @date Jan 24, 2023
 * @author Anton Bondarev
 */

#include <hal/platform.h>
#include <hal/reg.h>


#define FPU_CPACR  0xE000ED88
#define FPU_NSACR  0xE000ED8C
#define FPU_FPCCR  0xE000EF34

extern void board_clocks_init(void);

void platform_init(void) {
	board_clocks_init();

	/* Enable FPU */
	/* FIXME: Currently FPU is enabled permanently */
	REG32_ORIN(FPU_CPACR, 0xf << 20);
	/* Set CP0, CP1 Full Access in Secure mode (enable PowerQuad) */
	REG32_ORIN(FPU_CPACR, (3UL << 0 * 2) | (3UL << 1 * 2));
	REG32_ORIN(FPU_NSACR, 0xf << 20);
	/* Enable CP0, CP1, CP10, CP11 Non-secure Access  */
	REG32_ORIN(FPU_NSACR, (3UL << 0 * 2) | (3UL << 1 * 2));

	/* Disable FPU context preservation/restoration on exception
	 * entry and exit, because we can guarantee every irq handler
	 * execute without using FPU */
	REG32_CLEAR(FPU_FPCCR, 0x3 << 30);
}
