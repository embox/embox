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

#include <drivers/at91sam7s256.h>
#include <drivers/pins.h>
#include <drivers/bluetooth.h>
#include <unistd.h>
#include <string.h>
#include <kernel/panic.h>

#define MSG_BEGIN_INQUIRY                0
#define MSG_CANCEL_INQUIRY               1
#define MSG_CONNECT                      2
#define MSG_OPEN_PORT                    3
#define MSG_LOOKUP_NAME                  4
#define MSG_ADD_DEVICE                   5
#define MSG_REMOVE_DEVICE                6
#define MSG_DUMP_LIST                    7
#define MSG_CLOSE_CONNECTION             8
#define MSG_ACCEPT_CONNECTION            9
#define MSG_PIN_CODE                    10
#define MSG_OPEN_STREAM                 11
#define MSG_START_HEART                 12
#define MSG_HEARTBEAT                   13
#define MSG_INQUIRY_RUNNING             14
#define MSG_INQUIRY_RESULT              15
#define MSG_INQUIRY_STOPPED             16
#define MSG_LOOKUP_NAME_RESULT          17
#define MSG_LOOKUP_NAME_FAILURE         18
#define MSG_CONNECT_RESULT              19
#define MSG_RESET_INDICATION            20
#define MSG_REQUEST_PIN_CODE            21
#define MSG_REQUEST_CONNECTION          22
#define MSG_LIST_RESULT                 23
#define MSG_LIST_ITEM                   24
#define MSG_LIST_DUMP_STOPPED           25
#define MSG_CLOSE_CONNECTION_RESULT     26
#define MSG_PORT_OPEN_RESULT            27
#define MSG_SET_DISCOVERABLE            28
#define MSG_CLOSE_PORT                  29
#define MSG_CLOSE_PORT_RESULT           30
#define MSG_PIN_CODE_ACK                31
#define MSG_DISCOVERABLE_ACK            32
#define MSG_SET_FRIENDLY_NAME           33
#define MSG_SET_FRIENDLY_NAME_ACK       34
#define MSG_GET_LINK_QUALITY            35
#define MSG_LINK_QUALITY_RESULT         36
#define MSG_SET_FACTORY_SETTINGS        37
#define MSG_SET_FACTORY_SETTINGS_ACK    38
#define MSG_GET_LOCAL_ADDR              39
#define MSG_GET_LOCAL_ADDR_RESULT       40
#define MSG_GET_FRIENDLY_NAME           41
#define MSG_GET_DISCOVERABLE            42
#define MSG_GET_PORT_OPEN               43
#define MSG_GET_FRIENDLY_NAME_RESULT    44
#define MSG_GET_DISCOVERABLE_RESULT     45
#define MSG_GET_PORT_OPEN_RESULT        46
#define MSG_GET_VERSION                 47
#define MSG_GET_VERSION_RESULT          48
#define MSG_GET_BRICK_STATUSBYTE_RESULT 49
#define MSG_SET_BRICK_STATUSBYTE_RESULT 50
#define MSG_GET_BRICK_STATUSBYTE        51
#define MSG_SET_BRICK_STATUSBYTE        52

typedef struct {
	uint8_t  length;
	uint8_t  type;
	uint8_t  content[256];
	uint16_t sum;
} bt_message_t;

bt_message_t msg_queue;

bt_message_t out_msg;

uint8_t bt_tx_buff[256];

extern void bt_set_uart_state(void);

static char pin_code[] = CONFIG_BLUETOOTH_PIN;

bt_message_t *add(void) {
	return &(msg_queue);
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
#if 0
	TRACE("P%x:", msg->type);
	for (int i = 0; i < msg->length; i++) {
		TRACE("%x:", msg->content[i]);
	}
	TRACE("\n");
#endif

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
			return; //XXX
		}
		break;
	default:
		return;
	}
	len = bt_wrap(&out_msg, bt_tx_buff);
	nxt_bluetooth_write(bt_tx_buff, len);
}

void bt_handle(uint8_t *buff) {
	bt_message_t *msg = add();
	bt_unwrap(msg, buff);
	process_msg(msg);
}
