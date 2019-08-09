/**
 * @file
 *
 * @brief
 *
 * @date 30.10.2011
 * @author Anton Bondarev
 */

#include <string.h>
#include <embox/unit.h>
#include <util/log.h>

#include <drivers/bluetooth/lego/blue_core4.h>
#include <drivers/bluetooth/bluetooth.h>

#include <pnet/core/core.h>
#include <pnet/core/node.h>
#include <pnet/core/repo.h>
#include <pnet/pack/pnet_pack.h>
#include <pnet/pack/pack_alone.h>

#include <framework/mod/options.h>

#define CONFIG_BLUETOOTH_PIN OPTION_STRING_GET(bluetooth_pin)

EMBOX_UNIT_INIT(nxt_bluecore_init);

static int nxt_bluecore_start(struct net_node *node);
static int data_rx(struct pnet_pack *pack);
static int ctrl_rx(struct pnet_pack *pack);

PNET_NODE_DEF_NAME(BLUETOOTH_DRV_BLUE_CORE4_DATA, this_data, {
	.rx_hnd = data_rx,
	.start = nxt_bluecore_start
});

PNET_NODE_DEF_NAME(BLUETOOTH_DRV_BLUE_CORE4_CTRL, this_ctrl, {
	.rx_hnd = ctrl_rx,
});

static int get_length(void *msg);
static int get_body(void *msg);
static int bypass(void *msg);
static int wait_connect(void *msg);
static int wait_disconnect(void *msg);

static int (*data_hnd)(void *pack_data) = get_length;
static int (*ctrl_hnd)(void *pack_data) = wait_connect;

static struct bc_msg out_msg;

static uint16_t calc_chksumm(struct bc_msg * msg) {
	uint16_t sum = 0;
	int i;
	uint8_t *buffer = (uint8_t *)&(msg->type);

	for (i = 0; i < msg->length; buffer++, i++) {
		sum += *buffer;
	}
	sum = ~sum + 1;

	*buffer++ = sum >> 8;
	*buffer++ = sum & 0xff;
	return sum;
}

static void send_msg(struct bc_msg * msg) {
	calc_chksumm(msg);

	msg->length += 2;
	bluetooth_write((uint8_t *) msg, msg->length + 1);
}

static int process_msg(struct bc_msg_body *msg) {
	/* TODO it must be return in connect result we can also get partner address
	 * when we get port open result message */
	static int bt_bc_handle = 0;

	int res = 0;
	log_debug("P%x:", msg->type);

	switch (msg->type) {
	case MSG_RESET_INDICATION:
		out_msg.type = MSG_OPEN_PORT;
		out_msg.length = 1;
		break;
	case MSG_PORT_OPEN_RESULT:
		if (msg->content[0]) {
			bt_bc_handle = msg->content[1];
		}
		/* TODO nxt bt if coudn't may be reset the chip */
		return res;
	case MSG_REQUEST_PIN_CODE:
		out_msg.length = 1 + 7 + 16; /*type + BT addr + pin_code*/
		out_msg.type = MSG_PIN_CODE;
		memcpy(&out_msg.content, &(msg->content), 7);
		memset(out_msg.content + 7, 0, 17);
		memcpy(out_msg.content + 7, CONFIG_BLUETOOTH_PIN, 16);
		break;
	case MSG_REQUEST_CONNECTION:
		out_msg.type = MSG_ACCEPT_CONNECTION;
		out_msg.length = 2;
		out_msg.content[0] = 1;
		break;
	case MSG_CONNECT_RESULT:
		if (msg->content[0]) {
			out_msg.type = MSG_OPEN_STREAM;
			out_msg.length = 2;
			out_msg.content[0] = bt_bc_handle;
			bluetooth_hw_accept_connect();
			res = 1;
		} else {
			/* TODO nxt bt if coudn't may be reset the chip */
			return res;
		}
		break;
	default:
		return res;
	}
	send_msg(&out_msg);
	return res;
}

static int data_rx(struct pnet_pack *pack) {
	return data_hnd(pnet_pack_get_data(pack));
}

static int ctrl_rx(struct pnet_pack *pack) {
	return ctrl_hnd(pnet_pack_get_data(pack));

}

static int get_length(void *_msg) {
	struct bc_msg *msg = _msg;
	data_hnd = get_body;
	bluetooth_read(msg->length);

	return NET_HND_STOP_FREE;
}

static int get_body(void *msg) {
	int answ = process_msg((struct bc_msg_body *) msg);
	if (answ == 0) {
		data_hnd = get_length;
		bluetooth_read(1);
	}

	return NET_HND_STOP_FREE;
}

static int bypass(void *msg) {
	return NET_HND_FORWARD_DEFAULT;
}

static int wait_connect(void *msg) {
	ctrl_hnd = wait_disconnect;
	data_hnd = bypass;

	return NET_HND_FORWARD_DEFAULT;
}

static int wait_disconnect(void *msg) {
	data_hnd = get_length;
	ctrl_hnd = wait_connect;

	bluetooth_hw_soft_reset();
	bluetooth_read(1);

	return NET_HND_FORWARD_DEFAULT;
}

static int nxt_bluecore_start(struct net_node *node) {
	data_hnd = get_length;
	ctrl_hnd = wait_connect;

	bluetooth_hw_hard_reset();
	bluetooth_read(1);

	return 0;
}

static int nxt_bluecore_init(void) {
	return 0;
}
