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

static int dc_hnd_body(int msg, uint8_t *buff);
static int dc_hnd_size(int msg, uint8_t *buff);
static int dc_hnd_conn(int msg, uint8_t *buff);

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

static int dc_hnd_body(int msg, uint8_t *buff) {
	if (msg != BT_DRV_MSG_READ) {
		CALLBACK_REG(bluetooth_uart, (callback_t) dc_hnd_conn);
		return 0;
	}
	send_to_net(direct_comm_buff, MSG_SIZE_BYTE_CNT + size);
	CALLBACK_REG(bluetooth_uart, (callback_t) dc_hnd_size);
	bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
	return 0;
}

static int dc_hnd_size(int msg, uint8_t *buff) {
	if (msg != BT_DRV_MSG_READ) {
		CALLBACK_REG(bluetooth_uart, (callback_t) dc_hnd_conn);
		return 0;
	}
	size = handle_size(buff);
	CALLBACK_REG(bluetooth_uart, (callback_t) dc_hnd_body);
	bluetooth_read(direct_comm_buff + MSG_SIZE_BYTE_CNT, size);
	return 0;
}

static int dc_hnd_conn(int msg, uint8_t *buff) {
	CALLBACK_REG(bluetooth_uart, (callback_t) dc_hnd_size);
	bluetooth_read(direct_comm_buff, MSG_SIZE_BYTE_CNT);
	return 0;
}
#if 0
static int bt_xmit(sk_buff_t *skb, struct net_device *dev) {

	return ENOERR;
}

static net_device_stats_t * bt_get_stats(struct net_device *dev) {
	if (dev == NULL) {
		return NULL;
	}

	return &(dev->stats);
}

static const struct net_device_ops bt_ops = {
	.ndo_start_xmit      = bt_xmit,
	.ndo_get_stats       = bt_get_stats,
};

static int rebuild(sk_buff_t *pack) {
	return 0;
}

static const struct header_ops header_ops = {
	.rebuild = rebuild
};

static void bt_setup(struct net_device *dev) {
	dev->mtu                = 1024;
	dev->addr_len           = 0;
	dev->tx_queue_len       = 0;
	dev->type               = 0;
	dev->flags              = 0;
	dev->netdev_ops         = &bt_ops;
	dev->header_ops         = &header_ops;
}
static net_node_t bt_node = NULL;
#endif

static struct net_node bt_node;

PNET_NODE_STATIC_DEF("lego_nxt_bt", bt_node);

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

#endif
	pnet_node_init(&bt_node, 0, NULL);
	CALLBACK_REG(bluetooth_uart, (callback_t) dc_hnd_conn);

	return 0;
}
