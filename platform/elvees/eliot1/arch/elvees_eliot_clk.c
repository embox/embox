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


#define CLOCK_BASE ((struct clkctr_regs *)CLKCTR_BASE)

void board_clocks_init(void) {
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

int clk_enable(char *clk_name) {
	return 0;
}
