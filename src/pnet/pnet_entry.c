/**
 * @file
 * @brief
 *
 * @date 27.10.11
 * @author Anton Kozlov
 */

#include <errno.h>

#include <pnet/prior_path.h>

#include <net/skbuff.h>
#include <net/netdevice.h>

#include <pnet/core.h>
#include <pnet/node.h>

static net_node_t pnet_get_dev_by_device(struct net_device *dev) {
	net_node_t node = &dev->net_node;

	if (NULL == node) {
		return pnet_dev_get_entry();
	}

	return node;
}

int netif_rx(struct sk_buff *skb) {
	net_device_t *dev;

	if (NULL == skb) {
		return NET_RX_DROP;
	}

	dev = skb->dev;

	if (NULL == dev) {
		kfree_skb(skb);
		return NET_RX_DROP;
	}

	skb_queue_tail(&(dev->dev_queue), skb);
	netif_rx_schedule(dev);
	return NET_RX_SUCCESS;
}

int netif_receive_skb(sk_buff_t *skb) {
	net_packet_t pack;
	net_node_t node = pnet_get_dev_by_device(skb->dev);

        pack = pnet_pack_alloc(node, NET_PACKET_RX, skb->data, skb->len);

	pack->skbuf = skb;

	pnet_rx_thread_add(pack);

	return 0;
}

int dev_queue_xmit(struct sk_buff *skb) {
	int res;
	net_device_t *dev;
	const struct net_device_ops *ops;
	net_device_stats_t *stats;

	if (skb == NULL) {
		return -EINVAL;
	}

	dev = skb->dev;
	if (NULL == dev) {
		kfree_skb(skb);
		return -EINVAL;
	}

	ops = dev->netdev_ops;
	stats = ops->ndo_get_stats(dev);
	res = ops->ndo_start_xmit(skb, dev);
	if (res < 0) {
		kfree_skb(skb);
		stats->tx_err++;
		return res;
	}
	/* update statistic */
	stats->tx_packets++;
	stats->tx_bytes += skb->len;
	return ENOERR;
}
