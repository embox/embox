/**
 * @file
 * @brief Lego NXT Bluetooth helpers, layers for user level
 *
 * @date 11.03.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <unistd.h>
#include <string.h>
#include <kernel/panic.h>

#include <drivers/bluetooth.h>
#include <drivers/nxt_bluetooth.h>

bt_message_t in_msg;

bt_message_t out_msg;

uint16_t bt_mod_version = 0xffff; /*bt module version */

uint8_t bt_tx_buff[256];

extern void bt_set_uart_state(void);

static char pin_code[] = CONFIG_BLUETOOTH_PIN;

bt_message_t *add(void) {
	return &(in_msg);
}

static int bt_bc_handle = 0;

int bt_wrap(bt_message_t *header, uint8_t *buffer) {
	int i;
	uint16_t sum = 0;
	*buffer++ = header->length + 3;
	*buffer++ = header->type;
	sum += header->type;
	sum += header->length + 3; //is it need?
	for (i = 0; i < header->length; i++) {
		*buffer++ = header->content[i];
		sum += header->content[i];
	}
	sum = ~sum + 1;
	*buffer++ = sum >> 8;
	*buffer++ = sum & 0xff;
	return 1 + header->length + 3;
}

void bt_unwrap(bt_message_t *header, uint8_t *buffer) {
	int i;
	header->length = *buffer++;
	header->length -= 3;

	header->type = *buffer++;
	for (i = 0; i < header->length; i++) {
		header->content[i] = *buffer++;
	}
	header->sum = 0;
}

static void process_msg(bt_message_t *msg) {
	int len;
#if 0
	TRACE("P%x:", msg->type);
	for (int i = 0; i < msg->length; i++) {
		TRACE("%x:", msg->content[i]);
	}
	TRACE("\n");
#endif

	switch (msg->type) {
	case MSG_GET_VERSION_RESULT:
		bt_mod_version = (msg->content[0] << 8) + msg->content[1];
		break;
	case MSG_REQUEST_PIN_CODE:
		out_msg.length = 7 + 16; //BT addr + pin_code
		out_msg.type = MSG_PIN_CODE;
		memcpy(&out_msg.content, &(msg->content), 7);
		memset(out_msg.content + 7, 0, 17);
		memcpy(out_msg.content + 7, &pin_code, 16);
		break;
	case MSG_RESET_INDICATION:
		out_msg.type = MSG_OPEN_PORT;
		out_msg.length = 0;
		break;
	case MSG_PORT_OPEN_RESULT:
		if (msg->content[0]) {
			bt_bc_handle = msg->content[1];
		}
		return;
	case MSG_REQUEST_CONNECTION:
		out_msg.type = MSG_ACCEPT_CONNECTION;
		out_msg.length = 1;
		out_msg.content[0] = 1;
		break;
	case MSG_CONNECT_RESULT:
		if (msg->content[0]) {
			bt_set_uart_state();
			out_msg.type = MSG_OPEN_STREAM;
			out_msg.length = 1;
			out_msg.content[0] = bt_bc_handle;
		} else {
			return; //XXX
		}
		break;
	default:
		return;
	}
	len = bt_wrap(&out_msg, bt_tx_buff);
	bluetooth_write(bt_tx_buff, len);
}

void bt_handle(uint8_t *buff) {
	bt_message_t *msg = add();
	bt_unwrap(msg, buff);
	process_msg(msg);
}
