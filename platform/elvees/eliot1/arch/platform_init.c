/**
 * @file
 *
 * @date Jan 24, 2023
 * @author Anton Bondarev
 */

#include <hal/platform.h>
#include <hal/reg.h>

#include "clkctr.h"
#include "eliot1_board.h"

#define FPU_CPACR  0xE000ED88
#define FPU_NSACR  0xE000ED8C
#define FPU_FPCCR  0xE000EF34

#define CLOCK_BASE ((struct clkctr_regs *)CLKCTR_BASE)

static void board_clocks_init(void) {
	if (REG_LOAD(CPU_IDENTITY_REG) == 0) {
		clkctr_set_pll(CLOCK_BASE, 0, 0);
		clkctr_set_sys_div(CLOCK_BASE, 0,              /* FCLK == MAINCLK */
		    (BOARD_FCLK_FREQ / BOARD_SYSCLK_FREQ) - 1, /* SYSCLK DIV */
		    (BOARD_FCLK_FREQ / BOARD_GNSS_FREQ) - 1,   /* GNSSCLK DIV */
		    (BOARD_FCLK_FREQ / BOARD_QSPI_FREQ) - 1);  /* QSPI DIV */
		clkctr_set_pll(CLOCK_BASE, BOARD_XTI_FREQ,
		    (BOARD_FCLK_FREQ / BOARD_XTI_FREQ) - 1);
	}
}

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
