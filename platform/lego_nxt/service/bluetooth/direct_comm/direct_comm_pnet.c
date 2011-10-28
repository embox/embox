/**
 * @file
 * @brief Lego NXT direct command manager
 *
 * @date 13.03.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <unistd.h>
#include <string.h>
#include <kernel/panic.h>
#include <hal/reg.h>
#include <drivers/nxt/direct_comm.h>
#include <drivers/bluetooth.h>

#include <net/netdevice.h>
#include <net/skbuff.h>

#include <embox/unit.h>
static enum {
	COMM_HEADER,
	COMM_BODY,
} reader_state;

static int size = 0;

EMBOX_UNIT_INIT(nxt_direct_comm_init);

#define DC_BUFF_SIZE 40

static uint8_t direct_comm_buff[DC_BUFF_SIZE];

static int direct_comm_handle(int msg, uint8_t *buff);

static int nxt_direct_comm_init(void) {
	reader_state = COMM_HEADER;
	CALLBACK_REG(bluetooth_uart, (callback_t) direct_comm_handle);
	return 0;
}

static int handle_size(uint8_t *buff) {
	return buff[0] + (buff[1] << 8);
}

static void send_to_net(uint8_t *data, int len) {
	struct sk_buff *skb =alloc_skb(len, 0);
	memcpy(skb->data, data, len);
	netif_rx(skb);
	return;
}

static int direct_comm_handle(int msg, uint8_t *buff) {
	if (msg == BT_DRV_MSG_CONNECTED) {
	    bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
	} else if (msg == BT_DRV_MSG_READ) {
	    switch (reader_state) {
	    case COMM_HEADER:
		    reader_state = COMM_BODY;
		    size = handle_size(buff);

		    bluetooth_read(direct_comm_buff + MSG_SIZE_BYTE_CNT, size);

		    break;
	    case COMM_BODY:
		    reader_state = COMM_HEADER;

		    send_to_net(direct_comm_buff, MSG_SIZE_BYTE_CNT + size);

		    bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);

		    break;
	    default:
		    break;
	    }
	}
	return 0;
}
