/**
 * @file
 *
 * @brief
 *
 * @date 30.10.2011
 * @author Anton Bondarev
 */


#include <types.h>
#include <string.h>
#include <embox/unit.h>

#include <drivers/blue_core4.h>
#include <drivers/bluetooth.h>


EMBOX_UNIT_INIT(nxt_bluecore_init);

static struct bc_msg out_msg;
static struct bc_msg in_msg;

static uint16_t calc_chksumm(struct bc_msg * msg) {
	uint16_t sum;
	int i;
	uint8_t *buffer = (uint8_t *)msg;

	for (i = 0; i < (msg->length - 1); buffer++, i++) {
		sum += *buffer;
	}
	sum = ~sum + 1;

	*buffer++ = sum >> 8;
	*buffer++ = sum & 0xff;
	return sum;
}

static void send_msg(struct bc_msg * msg) {
	calc_chksumm(msg);
	bluetooth_write((uint8_t *) &out_msg, msg->length+1);
}


static void process_msg(struct bc_msg *msg) {
	/* TODO it must be return in connect result we can also get partner address
	 * when we get port open result message */
	static int bt_bc_handle = 0;

	switch (msg->type) {
	case MSG_RESET_INDICATION:
		out_msg.type = MSG_OPEN_PORT;
		out_msg.length = 0;
		break;
	case MSG_PORT_OPEN_RESULT:
		if (msg->content[0]) {
			bt_bc_handle = msg->content[1];
		}
		/* TODO nxt bt if coudn't may be reset the chip */
		return;
	case MSG_REQUEST_PIN_CODE:
		out_msg.length = 7 + 16; /*BT addr + pin_code*/
		out_msg.type = MSG_PIN_CODE;
		memcpy(&out_msg.content, &(msg->content), 7);
		memset(out_msg.content + 7, 0, 17);
		memcpy(out_msg.content + 7, CONFIG_BLUETOOTH_PIN, 16);
		break;
	case MSG_REQUEST_CONNECTION:
		out_msg.type = MSG_ACCEPT_CONNECTION;
		out_msg.length = 1;
		out_msg.content[0] = 1;
		break;
	case MSG_CONNECT_RESULT:
		if (msg->content[0]) {
			//bt_set_uart_state();
			out_msg.type = MSG_OPEN_STREAM;
			out_msg.length = 1;
			out_msg.content[0] = bt_bc_handle;
		} else {
			/* TODO nxt bt if coudn't may be reset the chip */
			return;
		}
		break;
	case MSG_GET_VERSION_RESULT: {
		uint16_t bt_mod_version = 0xffff; /*bt module version */
		bt_mod_version = (msg->content[0] << 8) + msg->content[1];
		return;
	}
	default:
		return;
	}
	send_msg(&out_msg);
}

static int irq_handler_get_length(void);
static int irq_handler_get_body(void);

static int irq_handler_get_length(void) {
	if(in_msg.length >= sizeof(in_msg)) {
		//error; reset?
	}
	nxt_bt_set_rx_handle(irq_handler_get_body);
	bluetooth_read((uint8_t *)&(in_msg.type), in_msg.length);
	return 0;
}

static int irq_handler_get_body(void) {
	process_msg(&in_msg);

	nxt_bt_set_rx_handle(irq_handler_get_length);
	bluetooth_read((uint8_t *)&(in_msg.type), 1);
	return 0;
}


static int nxt_bluecore_init(void) {
	nxt_bluetooth_reset();
	nxt_bt_set_rx_handle(irq_handler_get_length);
	bluetooth_read((uint8_t *)&(in_msg.type), 1);
	return 0;
}
