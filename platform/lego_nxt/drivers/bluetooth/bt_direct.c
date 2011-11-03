/**
 * @file
 *
 * @brief
 *
 * @date 28.10.2011
 * @author Anton Bondarev
 */

#include <types.h>

#include <drivers/nxt/direct_comm.h>
#include <drivers/bluetooth.h>

#include <pnet/core.h>
#include <pnet/repo.h>
#include <pnet/node.h>

#define DC_BUFF_SIZE 0x20 /* lego_nxt direct command maximum length */

static uint8_t direct_comm_buff[DC_BUFF_SIZE];

static void send_to_net(uint8_t *data, int len) {
//	struct sk_buff *skb = alloc_skb(len, 0);
//	memcpy(skb->data, data, len);
//	netif_rx(skb);
	return;
}
static int size = 0;
static int handle_size(uint8_t *buff) {
	return buff[0] + (buff[1] << 8);
}
static int direct_wait_body(void /*int msg, uint8_t *buff*/);
static int direct_get_header(void /*int msg, uint8_t *buff*/) {

	size = handle_size(direct_comm_buff);

	if (size > DC_BUFF_SIZE - MSG_SIZE_BYTE_CNT) {
		//TODO error length
	}
	CALLBACK_REG(bluetooth_uart, (callback_t) direct_wait_body);
	bluetooth_read(direct_comm_buff + MSG_SIZE_BYTE_CNT, size);
	return 0;
}

static int direct_wait_body(void /*int msg, uint8_t *buff*/) {
	send_to_net(direct_comm_buff, MSG_SIZE_BYTE_CNT + size);
	nxt_bt_set_rx_handle(direct_get_header);
	bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
	return 0;
}


static int nxt_direct_comm_init(struct net_node *node) {
	nxt_bt_set_rx_handle(direct_get_header);
	bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);

	return 0;
}

PNET_NODE_DEF("lego_blue_core",  {
		.start = nxt_direct_comm_init
	});
