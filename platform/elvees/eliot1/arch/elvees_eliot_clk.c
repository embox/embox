/**
 * @file
 *
 * @date Jan 24, 2023
 * @author Anton Bondarev
 */

 #include <stdint.h>

#include <hal/platform.h>
#include <hal/reg.h>

#include "clkctr.h"

#include <config/board_config.h>

#define CPU_IDENTITY_REG            0x4001F000

#define CLKCTRL_BASE ((struct clkctr_regs *)(uintptr_t)CONF_CLKCTRL_REGION_BASE)

#define BOARD_XTI_FREQ      CONF_CLKCTRL_CLK_ENABLE_XTI_FREQ()
#define BOARD_FCLK_FREQ     CONF_CLKCTRL_CLK_ENABLE_FCLK_FREQ()
#define BOARD_SYSCLK_FREQ   CONF_CLKCTRL_CLK_ENABLE_SYSCLK_FREQ()
#define BOARD_GNSS_FREQ     CONF_CLKCTRL_CLK_ENABLE_GNSS_FREQ()
#define BOARD_QSPI_FREQ     CONF_CLKCTRL_CLK_ENABLE_QSPI_FREQ()

void board_clocks_init(void) {
	if (REG_LOAD(CPU_IDENTITY_REG) == 0) {
		clkctr_set_pll(CLKCTRL_BASE, 0, 0);
		clkctr_set_sys_div(CLKCTRL_BASE, 0,              /* FCLK == MAINCLK */
		    (BOARD_FCLK_FREQ / BOARD_SYSCLK_FREQ) - 1, /* SYSCLK DIV */
		    (BOARD_FCLK_FREQ / BOARD_GNSS_FREQ) - 1,   /* GNSSCLK DIV */
		    (BOARD_FCLK_FREQ / BOARD_QSPI_FREQ) - 1);  /* QSPI DIV */
		clkctr_set_pll(CLKCTRL_BASE, BOARD_XTI_FREQ,
		    (BOARD_FCLK_FREQ / BOARD_XTI_FREQ) - 1);
	}
}

int clk_enable(char *clk_name) {
	return 0;
}
