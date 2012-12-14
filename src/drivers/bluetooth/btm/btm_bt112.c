/**
 * @file
 * @brief Implements BTM 112 high-level routines.
 *
 * @date 15.11.11
 * @author Anton Kozlov
 */

#include <drivers/bluetooth/bluetooth.h>
#include <drivers/bluetooth/btm112.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(btm112_init);

CALLBACK_INIT(nxt_bt_rx_handle_t, bt_rx);

typedef int (*string_handler)(int len, void *data);

enum state_num {
	CONNECT_WAIT = 0,
	LR_WAIT = 1,
	DISCONNECT_WAIT = 2
};

static const char *stamp[] = {
	"CONNECT",
	"\r\n",
	"DISCONNECT",
};

static int irq_hnd_wait_conn(int len, void *data);
static int irq_hnd_wait_lrlf(int len, void *data);
static int irq_hnd_wait_disconn(int len, void *data);

static string_handler str_hnds[] = {
	irq_hnd_wait_conn,
	irq_hnd_wait_lrlf,
	irq_hnd_wait_disconn,
};

static int rs_comm = 0;
static int rs_pos = 0;

static int set_state(int state_num) {
	rs_comm = state_num;
	rs_pos = 0;

	CALLBACK_REG(__bt_rx, str_hnds[state_num]);

	return 0;
}

static int general_handler(int cnt, void *data) {
	uint8_t *buff = (uint8_t *) data;

	while (cnt--) {
		if (*buff == stamp[rs_comm][rs_pos]) {
			rs_pos++;
		} else {
			rs_pos = 0;
		}
		if (stamp[rs_comm][rs_pos] == 0) {
			return 1;
		}
		buff++;
	}

	return 0;
}

static int irq_hnd_wait_conn(int len, void *data) {
	if (general_handler(len, data)) {
		set_state(LR_WAIT);
	}
	bluetooth_read(1);
	return 0;
}

static int irq_hnd_wait_lrlf(int len, void *data) {
	if (general_handler(len, data)) {
		set_state(DISCONNECT_WAIT);
		//Acknowlege about connect
		//FIXME
		CALLBACK(bt_state)();
		return 0;
	}
	bluetooth_read(1);
	return 0;
}

static int irq_hnd_wait_disconn(int len, void *data) {
	if (general_handler(len, data)) {
		set_state(CONNECT_WAIT);
		CALLBACK(bt_state)();
		bluetooth_read(1);
		return 0;
	}
	CALLBACK(bt_rx)(len, data);
	return 0;
}

static int btm112_init(void) {
	bluetooth_hw_hard_reset();
	CALLBACK_REG(__bt_rx, irq_hnd_wait_conn);
	bluetooth_read(1);
	return 0;
}
