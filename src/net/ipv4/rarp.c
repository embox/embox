/**
 * @file
 * @brief implementation of the RARP protocol
 *
 * @date 29.09.12
 * @author Ilia Vaprol
 */

#include <net/rarp.h>
#include <net/if_arp.h>
#include <string.h>
#include <errno.h>
#include <net/neighbour.h>
#include <assert.h>
#include <net/skbuff.h>
#include <embox/net/pack.h>
#include <net/inetdevice.h>

EMBOX_NET_PACK(ETH_P_RARP, rarp_rcv, rarp_init);

static int rarp_build(struct sk_buff *skb, unsigned short oper,
		unsigned short paddr_space, unsigned char haddr_len, unsigned char paddr_len,
		const unsigned char *src_haddr, const unsigned char *src_paddr,
		const unsigned char *dst_haddr, const unsigned char *dst_paddr,
		struct net_device *dev) {
	int ret;
	struct arpghdr *rarph;
	struct arpg_stuff rarph_stuff;

	assert(skb != NULL);
	assert((haddr_len != 0) && (src_haddr != NULL) && (dst_haddr != NULL));
	assert((paddr_len != 0) && (src_paddr != NULL) && (dst_paddr != NULL));
	assert(dev != NULL);

	/* Setup some fields */
	skb->dev = dev;
	skb->protocol = ETH_P_RARP;
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;

	/* Make device specific header */
	assert(dev->header_ops != NULL);
	assert(dev->header_ops->create != NULL);
	ret = dev->header_ops->create(skb, dev, skb->protocol,
			(void *)dst_haddr, (void *)src_haddr, skb->len);
	if (ret != ENOERR) {
		return ret;
	}

	/* Setup fixed-length fields */
	rarph = skb->nh.arpgh;
	assert(rarph != NULL);
	rarph->ha_space = htons(dev->type);
	rarph->pa_space = htons(paddr_space);
	rarph->ha_len = haddr_len;
	rarph->pa_len = paddr_len;
	rarph->oper = htons(oper);

	/* Setup variable-length fields */
	arpg_make_stuff(rarph, &rarph_stuff);
	memcpy(rarph_stuff.sha, src_haddr, haddr_len);
	memcpy(rarph_stuff.spa, src_paddr, paddr_len);
	memcpy(rarph_stuff.tha, dst_haddr, haddr_len);
	memcpy(rarph_stuff.tpa, dst_paddr, paddr_len);

	assert(ETH_HEADER_SIZE + ARPG_HEADER_SIZE(rarph) == skb->len); /* FIXME */

	return ENOERR;
}

static int rarp_xmit(struct sk_buff *skb) {
	/* fall through to dev layer */
	return dev_queue_xmit(skb);
}

int rarp_send(unsigned short oper, unsigned short paddr_space,
		unsigned char haddr_len, unsigned char paddr_len,
		const unsigned char *src_haddr, const unsigned char *src_paddr,
		const unsigned char *dst_haddr, const unsigned char *dst_paddr,
		struct net_device *dev) {
	int ret;
	struct sk_buff *skb;

	assert(dev != NULL);
	assert((haddr_len != 0) && (paddr_len != 0));

	/* check device flags */
	if (dev->flags & IFF_NOARP) {
		return -EINVAL;
	}

	/* allocate net package */
	skb = skb_alloc(ETH_HEADER_SIZE + ARPG_CALC_HDR_SZ(haddr_len, paddr_len));
	if (skb == NULL) {
		return -ENOMEM;
	}

	/* build package */
	ret = rarp_build(skb, oper, paddr_space, haddr_len, paddr_len,
			src_haddr, src_paddr, dst_haddr, dst_paddr, dev);
	if (ret != ENOERR) {
		skb_free(skb);
		return ret;
	}

	/* and send */
	return rarp_xmit(skb);
}

static int rarp_hnd_request(struct arpghdr *rarph, struct arpg_stuff *rarps,
		struct sk_buff *skb, struct net_device *dev) {
	int ret;
	unsigned char haddr_len, paddr_len;
	unsigned char haddr_src[MAX_ADDR_LEN], haddr_dest[MAX_ADDR_LEN];
	unsigned char paddr_src[MAX_ADDR_LEN], paddr_dest[MAX_ADDR_LEN];
	struct in_device *in_dev;

	/* save source and destination hardware addreesses */
	haddr_len = rarph->ha_len;
	memcpy(&haddr_src[0], rarps->tha, haddr_len);
	memcpy(&haddr_dest[0], rarps->sha, haddr_len);

	/* get destination protocol address */
	/*paddr_len = rarph->pa_len;*/
	ret = neighbour_get_protocol_address(&haddr_dest[0], haddr_len,
			dev, sizeof paddr_dest, &paddr_dest[0], &paddr_len);
	if (ret != ENOERR) {
		skb_free(skb);
		return ret;
	}

	in_dev = in_dev_get(dev);
	assert(in_dev != NULL);

	/* get source protocol address */
	assert(paddr_len == sizeof in_dev->ifa_address); /* FIXME */
	memcpy(&paddr_src[0], &in_dev->ifa_address, paddr_len);

	/* build reply */
	ret = rarp_build(skb, RARP_OPER_REPLY, ntohs(rarph->pa_space),
			haddr_len, paddr_len, &haddr_src[0], &paddr_src[0],
			&haddr_dest[0], &paddr_dest[0], dev);
	if (ret != ENOERR) {
		skb_free(skb);
		return ret;
	}

	/* and send */
	return rarp_xmit(skb);
}

static int rarp_hnd_reply(struct arpghdr *rarph, struct arpg_stuff *rarps,
		struct sk_buff *skb, struct net_device *dev) {
	int ret;

	assert(rarph != NULL);
	assert(rarps != NULL);

	/* save destination hardware and protocol addresses */
	ret = neighbour_add(rarps->sha, rarph->ha_len, rarps->spa, rarph->pa_len,
			dev, 0);
	skb_free(skb);
	return ret;
}

static int rarp_process(struct sk_buff *skb, struct net_device *dev) {
	struct arpghdr *rarph;
	struct arpg_stuff rarph_stuff;

	assert(skb != NULL);
	assert(dev != NULL);

	rarph = skb->nh.arpgh;
	assert(rarph != NULL);

	/* check hardware and protocol address lengths */
	if ((skb->nh.raw - skb->mac.raw) + ARPG_HEADER_SIZE(rarph) != skb->len) {
		skb_free(skb);
		return -1; /* error: bad packet */
	}

	/* check device capabilities */
	if ((rarph->ha_space != htons(dev->type))
			|| (rarph->ha_len != dev->addr_len)) {
		skb_free(skb);
		return -1; /* error: invalid hardware address info */
	}

	arpg_make_stuff(rarph, &rarph_stuff);

	/* check recipient by hardware address */
	if (memcmp(rarph_stuff.tha, &dev->dev_addr[0],
				rarph->ha_len) != 0) {
		skb_free(skb);
		return -1; /* error: not for us */
	}

	/* process the packet by the operation code */
	switch (ntohs(rarph->oper)) {
	default:
		skb_free(skb);
		return -1; /* error: bad operation type */
	case RARP_OPER_REQUEST:
		/* handling request */
		return rarp_hnd_request(rarph, &rarph_stuff, skb, dev);
	case RARP_OPER_REPLY:
		/* handling reply */
		return rarp_hnd_reply(rarph, &rarph_stuff, skb, dev);
	}
}

int rarp_rcv(struct sk_buff *skb, struct net_device *dev,
		struct packet_type *pt, struct net_device *orig_dev) {
	assert(skb != NULL);
	assert(dev != NULL);

	/* check recipient */
	switch (eth_packet_type(skb)) {
	default:
		break; /* error: not for us */
	case PACKET_HOST:
	case PACKET_BROADCAST:
	case PACKET_MULTICAST:
		/* check device flags */
		if (dev->flags & IFF_NOARP) {
			break; /* error: arp doesn't supported */
		}
		/* handle package */
		return ((rarp_process(skb, dev) == ENOERR)
				? NET_RX_SUCCESS /* handled successfully */
				: NET_RX_DROP); /* error occured */
	}

	/* pretend that it was not */
	skb_free(skb);
	return NET_RX_DROP;
}

static int rarp_init(void) {
	/* ummm.. done */
	return ENOERR;
}

