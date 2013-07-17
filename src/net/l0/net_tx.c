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
#include <net/neighbour.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stddef.h>
#include <util/array.h>

static int nt_build_hdr(struct sk_buff *skb,
		struct net_header_info *hdr_info,
		struct net_device *dev) {
	int ret;
	unsigned char dst_addr[MAX_ADDR_LEN];

	assert(skb != NULL);
	assert(dev != NULL);

	if (hdr_info == NULL) {
		return 0; /* ok: already built */
	}

	/* resolve hw address */
	if (hdr_info->src_addr == NULL) {
		hdr_info->src_addr = &dev->dev_addr[0];
	}
	if (hdr_info->dst_addr == NULL) {
		if (hdr_info->dst_paddr != NULL) {
			ret = neighbour_get_haddr(hdr_info->type,
					hdr_info->dst_paddr, dev, dev->type,
					ARRAY_SIZE(dst_addr), &dst_addr[0]);
			if (ret != 0) {
				return ret;
			}
			hdr_info->dst_addr = &dst_addr[0];
		}
		else {
			hdr_info->dst_addr = &dev->broadcast[0];
		}
	}

	/* try to build */
	assert(dev->ops != NULL);
	assert(dev->ops->build_hdr != NULL);
	return dev->ops->build_hdr(skb, hdr_info);
}

int net_tx(struct sk_buff *skb,
		struct net_header_info *hdr_info) {
	int ret;
	size_t skb_len;
	struct net_device *dev;

	if (skb == NULL) {
		return -EINVAL;
	}

	dev = skb->dev;
	assert(dev != NULL);

	if (!(dev->flags & IFF_UP)) {
		skb_free(skb);
		return -ENETDOWN;
	}

	if (0 != nt_build_hdr(skb, hdr_info, dev)) {
		assert(hdr_info != NULL);
		return neighbour_send_after_resolve(hdr_info->type,
				hdr_info->dst_paddr, hdr_info->dst_plen,
				dev, skb);
	}

	skb_len = skb->len;

	assert(dev->drv_ops != NULL);
	assert(dev->drv_ops->xmit != NULL);
	ret = dev->drv_ops->xmit(dev, skb);
	if (ret != 0) {
		skb_free(skb);
		dev->stats.tx_err++;
		return ret;
	}

	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb_len;

	return 0;
}
