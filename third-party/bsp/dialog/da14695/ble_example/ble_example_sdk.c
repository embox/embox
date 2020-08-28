#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <framework/cmd/api.h>

#include <config/custom_config_qspi.h>

#include <ble_common.h>
#include <ble_mgr.h>
#include <ble_gap.h>

/*
 * BLE adv demo advertising data
 */
static const uint8_t adv_data[] = {
	0xf, GAP_DATA_TYPE_LOCAL_NAME,
	'E', 'm', 'b', 'o', 'x', ' ', 'A', 'D', 'V', ' ', 'D', 'e', 'm', 'o'
};

#if 0
#define CMAC_SHARED_POWER_CTRL_REG_CONFIG_MSK                                                   \
        (REG_MSK(CRG_TOP, POWER_CTRL_REG, LDO_CORE_ENABLE) |                                    \
         REG_MSK(CRG_TOP, POWER_CTRL_REG, LDO_CORE_RET_ENABLE_ACTIVE) |                         \
         REG_MSK(CRG_TOP, POWER_CTRL_REG, VDD_LEVEL))

extern void cmac_update_power_ctrl_reg_values(uint32_t onsleep_value);
#endif
extern void ad_ble_init(void);

int main(int argc, char **argv) {
	ble_mgr_init();

	ad_ble_init();

#if 0
	/* FIXME */
	cmac_update_power_ctrl_reg_values(
		CRG_TOP->POWER_CTRL_REG & (~CMAC_SHARED_POWER_CTRL_REG_CONFIG_MSK));
#endif
	ble_peripheral_start();

	/* Set device name */
	ble_gap_device_name_set("Embox ADV Demo", ATT_PERM_READ);

	/* Set advertising data */
	ble_gap_adv_data_set(sizeof(adv_data), adv_data, 0, NULL);

	/* Start advertising */
	ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);

	while (1) {
		/* It's just to let other threads to execute. */
		usleep(1 * 1000);
	}

	return 0;
}
