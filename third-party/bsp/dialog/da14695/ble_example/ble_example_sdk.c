#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <framework/cmd/api.h>

#include <config/custom_config_qspi.h>

#include <ble_common.h>
#include <ble_mgr.h>
#include <ble_gap.h>
#include <hw_clk.h>
#include <sys_clock_mgr.h>

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
extern int deep_usleep(useconds_t usec);

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

	if (!cm_lp_clk_is_avail()) {
		fprintf(stderr, "No low power enabled\n");
		return -1;
	} else {
		ad_ble_lpclock_available();
	}

	while (1) {
	/* TODO This part can lead to potentially broken USB because of
	 * of disableing/enabling COM (GPIO), so disable it for a while. */
#if 0
		hw_sys_pd_com_disable();
		hw_sys_pd_periph_disable();

		deep_usleep(1 * 1000 * 1000);

		hw_sys_pd_periph_enable();
		hw_sys_pd_com_enable();
#else
		/* It's just to let other threads to execute. */
		sleep(3600);
#endif
	}

	return 0;
}
