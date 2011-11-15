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
#include <kernel/softirq.h>
#include <embox/unit.h>

#include <drivers/blue_core4.h>
#include <drivers/bluetooth.h>

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>

#include <kernel/prom_printf.h>

EMBOX_UNIT_INIT(nxt_bluecore_init);

static int nxt_bluecore_start(struct net_node *node);

PNET_NODE_DEF_NAME("lego_blue_core", this, {
		.start = nxt_bluecore_start
});

#define SOFTIRQ_DEFFERED_DISCONNECT 10

static struct bc_msg out_msg;
static struct bc_msg in_msg;

static void send_to_net(char *data, int len) {
	net_packet_t pack = pnet_pack_alloc(&this, NET_PACKET_RX, (void *) data, len);

	pnet_entry(pack);

	return;
}

static uint16_t calc_chksumm(struct bc_msg * msg) {
	uint16_t sum;
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
	bluetooth_write((uint8_t *) &out_msg, msg->length + 1);
}

static void print_msg(struct bc_msg *msg) {
#if 0
	prom_printf("P%x:", msg->type);
	for (int i = 0; i < msg->length - 1; i++) {
		prom_printf("%x:", msg->content[i]);
	}
	prom_printf("\n");
#endif
}

static int process_msg(struct bc_msg *msg) {
	/* TODO it must be return in connect result we can also get partner address
	 * when we get port open result message */
	static int bt_bc_handle = 0;

	int res = 0;
	print_msg(msg);

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
			send_to_net("connect", 1 + strlen("connect"));
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

static int irq_handler_get_length(void);
static int irq_handler_get_body(void);

static int irq_handler_get_length(void) {
	if(in_msg.length >= sizeof(in_msg)) {
		//error; reset?
	}
	CALLBACK_REG(bt_rx, irq_handler_get_body);
	bluetooth_read((uint8_t *)&(in_msg.type), in_msg.length);
	return 0;
}

static int irq_handler_get_body(void) {
	int answ = process_msg(&in_msg);

	if (answ == 0) {
		CALLBACK_REG(bt_rx, irq_handler_get_length);
		bluetooth_read((uint8_t *)&(in_msg.length), 1);
	}
	return 0;
}

static int pin_disconnect_handler(void) {
	softirq_raise(SOFTIRQ_DEFFERED_DISCONNECT);
	return 0;
}

static int nxt_bluecore_start(struct net_node *node) {
	bluetooth_hw_hard_reset();
	CALLBACK_REG(bt_rx, irq_handler_get_length);
	CALLBACK_REG(bt_state, pin_disconnect_handler);
	bluetooth_read((uint8_t *)&(in_msg.length), 1);
	return 0;
}

static void deffered_disconnect(softirq_nr_t nt, void* data) {
	bluetooth_hw_soft_reset();
	CALLBACK_REG(bt_rx, irq_handler_get_length);
	bluetooth_read((uint8_t *)&(in_msg.length), 1);
}

static int nxt_bluecore_init(void) {
	return softirq_install(SOFTIRQ_DEFFERED_DISCONNECT, deffered_disconnect, NULL);
}

