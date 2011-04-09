/*
 * @file
 *
 * @brief Lego NXT Bluetooth helpers, layers for user level
 *
 * @date 11.03.2011
 * @author Anton Kozlov
 */
#include <types.h>
#include <embox/unit.h>
#include <hal/reg.h>

#include <drivers/at91sam7s256.h>
#include <drivers/pins.h>
#include <drivers/bluetooth.h>
#include <unistd.h>
#include <string.h>
#include <kernel/panic.h>

#define MSG_QUEUE_SIZE 8
bt_message_t msg_queue[MSG_QUEUE_SIZE];
int qr = 0;
int qn = 0;

bt_message_t out_msg;

uint8_t bt_tx_buff[256];

static char pin_code[] = "1234";

bt_message_t *add(void) {
#if 0
	if (++qn == MSG_QUEUE_SIZE) {
		panic("BT msg buff overflow");
	}
#endif
	if (++qr >= MSG_QUEUE_SIZE) {
		qr = 0;
	}
	return &(msg_queue[qr]);
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

static void bt_unwrap(bt_message_t *header, uint8_t *buffer) {
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
	TRACE("P%x:", msg->type);
	for (int i = 0; i < msg->length; i++) {
		TRACE("%x:", msg->content[i]);
	}
	TRACE("\n");

	switch (msg->type) {
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
			break;
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
				REG_STORE(AT91C_US1_IER, AT91C_US_ENDTX);
			} else {
				return;
			}
			break;
		default:
			return;
			break;
	}
	len = bt_wrap(&out_msg, bt_tx_buff);
	nxt_bluetooth_write(bt_tx_buff, len);
}

void bt_handle(uint8_t *buff) {
	bt_message_t *msg = add();
	bt_unwrap(msg, buff);
	process_msg(msg);
}

