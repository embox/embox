#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <framework/cmd/api.h>

#include <config/custom_config_qspi.h>

#include <ble_common.h>
#include <ble_mgr.h>
#include <ble_gap.h>
#include <sys_clock_mgr.h>

/*
 * BLE adv demo advertising data
 */
static const uint8_t adv_data[] = {
	0xf, GAP_DATA_TYPE_LOCAL_NAME,
	'E', 'm', 'b', 'o', 'x', ' ', 'A', 'D', 'V', ' ', 'D', 'e', 'm', 'o'
};

extern void ad_ble_init(void);

int main(int argc, char **argv) {
	ble_mgr_init();

	ad_ble_init();

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
		/* It's just to let other threads to execute. */
		sleep(3600);
	}

	return 0;
}
