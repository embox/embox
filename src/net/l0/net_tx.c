/**
 * @file
 * @brief
 *
 * @date 19.06.11
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <net/l3/arp_queue.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stddef.h>

int dev_send_skb(struct sk_buff *skb) {
	int ret;

	assert(skb != NULL);
	assert(skb->dev != NULL);
	assert(skb->dev->ops != NULL);
	assert(skb->dev->ops->rebuild_hdr != NULL);
	ret = skb->dev->ops->rebuild_hdr(skb);
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

	assert(dev->drv_ops->xmit != NULL);
	ret = dev->drv_ops->xmit(dev, skb);
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
