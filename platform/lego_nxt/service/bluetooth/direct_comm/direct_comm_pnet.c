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

#include <embox/unit.h>
static enum {
	COMM_HEADER,
	COMM_BODY,
} reader_state;


EMBOX_UNIT_INIT(nxt_direct_comm_init);

#define DC_BUFF_SIZE 40

static uint8_t direct_comm_buff[DC_BUFF_SIZE];

static int size = 0;
static int handle_size(uint8_t *buff) {
	return buff[0] + (buff[1] << 8);
}

static void send_to_net(uint8_t *data, int len) {
	struct sk_buff *skb = alloc_skb(len, 0);
	memcpy(skb->data, data, len);
	//skb->net_node;
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

static int nxt_direct_comm_init(void) {
	struct net_device *dev = alloc_netdev(0, "nxt_bt", bt_setup);
	if (dev == NULL) {
		return -ENOMEM;
	}

	bt_node = pnet_dev_register(dev);

	reader_state = COMM_HEADER;
	CALLBACK_REG(bluetooth_uart, (callback_t) direct_comm_handle);

	return 0;
}
