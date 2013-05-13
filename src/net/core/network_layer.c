/**
 * @file
 * @brief
 *
 * @date 19.06.11
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <string.h>
#include <stdio.h>
#include <embox/unit.h>
#include <linux/list.h>
#include <net/inetdevice.h>
#include <linux/interrupt.h>
#include <kernel/irq.h>
#include <errno.h>
#include <net/sock.h>
#include <net/route.h>
#include <net/arp_queue.h>
#include <framework/net/pack/api.h>
#include <embox/net/pack.h>

EMBOX_UNIT_INIT(unit_init);

int dev_send_skb(struct sk_buff *skb) {
	int ret;

	assert(skb != NULL);
	assert(skb->dev != NULL);
	assert(skb->dev->header_ops != NULL);
	assert(skb->dev->header_ops->rebuild != NULL);
	ret = skb->dev->header_ops->rebuild(skb);
	if (ret != 0) {
		ret = arp_queue_wait_resolve(skb);
		if (ret != 0) {
			skb_free(skb);
			return ret;
		}
		return 0;
	}

	return dev_xmit_skb(skb);
}

int dev_xmit_skb(struct sk_buff *skb) {
	int ret;
	unsigned int skb_len;
	struct net_device *dev;

	assert(skb != NULL);

	dev = skb->dev;
	assert(dev != NULL);

	if (!(dev->flags & IFF_UP)) {
		skb_free(skb);
		return -ENETDOWN;
	}

	skb_len = skb->len;

	assert(dev->netdev_ops->ndo_start_xmit != NULL);
	ret = dev->netdev_ops->ndo_start_xmit(skb, dev);
	if (ret != 0) {
		skb_free(skb);
		dev->stats.tx_err++;
		return ret;
	}

	/* update statistic */
	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb_len;

	return 0;
}

int netif_receive_skb(struct sk_buff *skb) {
	const struct net_pack *npack;

	assert(skb != NULL);

	npack = net_pack_lookup(skb->protocol);
	if (npack == NULL) {
		skb_free(skb);
		return NET_RX_DROP;
	}

	return npack->handle(skb, skb->dev);
}

void netif_rx_schedule(struct sk_buff *skb) {
	struct net_device *dev;

	assert(skb != NULL);

	dev = skb->dev;
	assert(dev != NULL);

	skb_queue_push(&(dev->dev_queue), skb);

	netdev_rx_queued(dev);

	raise_softirq(NET_RX_SOFTIRQ);
}

static void netif_rx_action(struct softirq_action *action) {
	netdev_rx_processing();
}

static int unit_init(void) {
	open_softirq(NET_RX_SOFTIRQ, netif_rx_action, NULL);
	return 0;
}
