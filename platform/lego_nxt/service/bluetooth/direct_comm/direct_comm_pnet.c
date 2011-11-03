/**
 * @file
 * @brief Lego NXT direct command manager
 *
 * @date 13.03.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <kernel/panic.h>
#include <hal/reg.h>
#include <drivers/nxt/direct_comm.h>
#include <drivers/bluetooth.h>

#include <net/netdevice.h>
#include <net/skbuff.h>

#include <pnet/core.h>
#include <pnet/repo.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(nxt_direct_comm_init);

#define DC_BUFF_SIZE 40

static uint8_t direct_comm_buff[DC_BUFF_SIZE];

#if 0
static struct net_device *dev;
#endif

static int dc_hnd_body(void);
static int dc_hnd_size(void);

static int size = 0;
static int handle_size(uint8_t *buff) {
	return buff[0] + (buff[1] << 8);
}

static void send_to_net(uint8_t *data, int len) {
	net_packet_t pack = pnet_pack_alloc(NULL, NET_PACKET_RX, (void *) data, len);

	memcpy(pnet_pack_get_data(pack), data, len);

	pnet_entry(pack);

	return;
}

static int dc_hnd_body(void) {
	send_to_net(direct_comm_buff, MSG_SIZE_BYTE_CNT + size);
	CALLBACK_REG(bluetooth_uart, (callback_t) dc_hnd_size);
	bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
	return 0;
}

static int dc_hnd_size(void) {
	size = handle_size(direct_comm_buff);
	CALLBACK_REG(bluetooth_uart, (callback_t) dc_hnd_body);
	bluetooth_read(direct_comm_buff + MSG_SIZE_BYTE_CNT, size);
	return 0;
}

static int rx_hnd(net_packet_t pack) {
	if (strcmp("connect", pnet_pack_get_data(pack))) {
		nxt_bt_set_rx_handle(dc_hnd_size);
		bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
	}
	return 0;
}


PNET_NODE_DEF("lego_nxt_bt", {
	.rx_hnd = rx_hnd
});

static int nxt_direct_comm_init(void) {
#if 0
	dev = alloc_netdev(0, "nxt_bt", bt_setup);
	if (dev == NULL) {
		return -ENOMEM;
	}

	pnet_node_init(&bt_node,

	if (0 != register_netdev(dev) ) {
		return -EINVAL;
	}

	bt_node = pnet_dev_register(dev);

	pnet_node_init(&bt_node, 0, NULL);
	CALLBACK_REG(bluetooth_uart, (callback_t) dc_hnd_conn);
#endif

	return 0;
}
