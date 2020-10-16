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

static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt) {
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt) {
	/* Restart advertising */
	ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
}

static void handle_evt_gap_pair_req(ble_evt_gap_pair_req_t *evt) {
	ble_gap_pair_reply(evt->conn_idx, true, evt->bond);
}

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

	/* TODO The only difference from the original ble_adv example
	 * is removed watchdog. */
	for (;;) {
		ble_evt_hdr_t *hdr;

		/*
		 * Wait for a BLE event - this task will block
		 * indefinitely until something is received.
		 */
		hdr = ble_get_event(true);

		if (!hdr) {
			continue;
		}

		switch (hdr->evt_code) {
		case BLE_EVT_GAP_CONNECTED:
			handle_evt_gap_connected((ble_evt_gap_connected_t *) hdr);
			break;
		case BLE_EVT_GAP_DISCONNECTED:
			handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
			break;
		case BLE_EVT_GAP_PAIR_REQ:
			handle_evt_gap_pair_req((ble_evt_gap_pair_req_t *) hdr);
			break;
		default:
			ble_handle_event_default(hdr);
			break;
		}

		/* Free event buffer (it's not needed anymore) */
		OS_FREE(hdr);
	}

	return 0;
}
