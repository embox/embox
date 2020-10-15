/**
 * @file
 *
 * @date   14.10.2020
 * @author Alexander Kalmuk
 */

#include <config/custom_config_qspi.h>
#include <sys_clock_mgr.h>
#include <hw_clk.h>
#include <ad_ble.h>

static bool lp_clk_is_avail = false;

bool cm_lp_clk_is_avail(void) {
	return lp_clk_is_avail;
}

void lp_clock_enable(void) {
	int volatile i;

	/* LP clocks allows BLE to go sleep as I seen in ad_ble.c */
	hw_clk_set_lpclk(LP_CLK_IS_XTAL32K);

	for (i = 0; i < 1000000; i++) {
	}

	lp_clk_is_avail = true;
}
