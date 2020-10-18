#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <framework/cmd/api.h>

#include <config/custom_config_qspi.h>

#include "sdk_defs.h"

#include "ble_config.h"
#include "ble_common.h"
#include "ble_mgr.h"
#include "ble_mgr_common.h"
#include "ble_mgr_ad_msg.h"
#include "ble_mgr_gtl.h"
#include "ble_mgr_gap.h"

#include <sys_clock_mgr.h>

extern void ble_platform_initialization(void);
extern int ble_example_adv_start(void);
extern void cmac_update_power_ctrl_reg_values(uint32_t onsleep_value);

#define CMAC_SHARED_POWER_CTRL_REG_CONFIG_MSK                                                   \
        (REG_MSK(CRG_TOP, POWER_CTRL_REG, LDO_CORE_ENABLE) |                                    \
         REG_MSK(CRG_TOP, POWER_CTRL_REG, LDO_CORE_RET_ENABLE_ACTIVE) |                         \
         REG_MSK(CRG_TOP, POWER_CTRL_REG, VDD_LEVEL))

int main(int argc, char **argv) {
	int lp_clk;

	lp_clk = REG_GETF(CRG_TOP, CLK_RC32K_REG, RC32K_ENABLE);
	assert(lp_clk != 0);

	ble_mgr_init();

	ble_mgr_dev_params_set_default();

	ble_platform_initialization();

	// TODO ble_example_pmu_init();
	cmac_update_power_ctrl_reg_values(
		CRG_TOP->POWER_CTRL_REG & (~CMAC_SHARED_POWER_CTRL_REG_CONFIG_MSK));

	ble_example_adv_start();

	return 0;
}
