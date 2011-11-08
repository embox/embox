/*
 * @file
 * @brief Lego NXT direct command manager
 *
 * @date 13.03.11
 * @author Anton Kozlov
 */
#include <types.h>
#include <string.h>

#include <drivers/nxt/direct_comm.h>
#include <drivers/bluetooth.h>

#include <pnet/core.h>
#include <pnet/repo.h>
#include <pnet/node.h>

#define DC_BUFF_SIZE 0x20 /* lego_nxt direct command maximum length */

static uint8_t direct_comm_buff[DC_BUFF_SIZE];


static void send_to_net(unsigned char *data, int len) {
	net_packet_t pack = pnet_pack_alloc(NULL, NET_PACKET_RX, (void *) data, len);

	memcpy(pnet_pack_get_data(pack), data, len);

	pnet_entry(pack);

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
	nxt_bt_set_rx_handle(direct_wait_body);
	bluetooth_read(direct_comm_buff + MSG_SIZE_BYTE_CNT, size);
	return 0;
}

static int direct_wait_body(void /*int msg, uint8_t *buff*/) {
	send_to_net(direct_comm_buff, MSG_SIZE_BYTE_CNT + size);
	nxt_bt_set_rx_handle(direct_get_header);
	bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
	return 0;
}


static int rx_hnd(net_packet_t pack) {
	int res = NET_HND_SUPPRESSED;
	if (!strcmp("connect", pnet_pack_get_data(pack))) {
		return res;
	}
//	prom_printf("C");
	nxt_bt_set_rx_handle(direct_get_header);
	bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
	return res;
}


PNET_NODE_DEF("nxt direct src", {
	.rx_hnd = rx_hnd
});
