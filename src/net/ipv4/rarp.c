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
		const unsigned char *target_haddr, struct net_device *dev) {
	int ret;
	struct arpghdr *rarph;
	struct arpg_stuff rarph_stuff;

	assert(skb != NULL);
	assert((haddr_len != 0) && (paddr_len != 0));
	assert((src_haddr != NULL) && (dst_haddr != NULL));
	assert(dev != NULL);

	/* Get default arguments */
	assert(haddr_len == dev->addr_len);
	target_haddr = ((target_haddr != NULL) ? target_haddr : &dev->broadcast[0]);

	/* Setup some fields */
	skb->dev = dev;
	skb->protocol = ETH_P_RARP;
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;

	/* Make device specific header */
	assert(dev->header_ops != NULL);
	assert(dev->header_ops->create != NULL);
	ret = dev->header_ops->create(skb, dev, skb->protocol,
			(void *)target_haddr, (void *)src_haddr, skb->len);
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
	if (src_paddr != NULL) {
		memcpy(rarph_stuff.spa, src_paddr, paddr_len);
	}
	else {
		memset(rarph_stuff.spa, 0, paddr_len);
	}
	memcpy(rarph_stuff.tha, dst_haddr, haddr_len);
	if (dst_paddr != NULL) {
		memcpy(rarph_stuff.tpa, dst_paddr, paddr_len);
	}
	else {
		memset(rarph_stuff.tpa, 0, paddr_len);
	}

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
		const unsigned char *target_haddr, struct net_device *dev) {
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
			src_haddr, src_paddr, dst_haddr, dst_paddr, target_haddr, dev);
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
	unsigned char haddr_source[MAX_ADDR_LEN], paddr_source[MAX_ADDR_LEN];
	unsigned char haddr_dest[MAX_ADDR_LEN], paddr_dest[MAX_ADDR_LEN];
	unsigned char haddr_target[MAX_ADDR_LEN];
	struct in_device *in_dev;

	in_dev = in_dev_get(dev);
	assert(in_dev != NULL);

	haddr_len = rarph->ha_len;
	paddr_len = rarph->pa_len;

	/* get source addresses */
	memcpy(&haddr_source[0], &dev->dev_addr[0], haddr_len);
	assert(paddr_len == sizeof in_dev->ifa_address); /* FIXME */
	memcpy(&paddr_source[0], &in_dev->ifa_address, paddr_len);

	/* get dest addresses */
	memcpy(&haddr_dest[0], rarps->tha, haddr_len);
	ret = neighbour_get_protocol_address(&haddr_dest[0], haddr_len,
			dev, paddr_len, &paddr_dest[0], NULL);
	if (ret != ENOERR) {
		skb_free(skb);
		return ret;
	}

	/* get target hardware address */
	memcpy(&haddr_target[0], rarps->sha, haddr_len);

	/* build reply */
	ret = rarp_build(skb, RARP_OPER_REPLY, ntohs(rarph->pa_space),
			haddr_len, paddr_len, &haddr_source[0], &paddr_source[0],
			&haddr_dest[0], &paddr_dest[0], &haddr_target[0], dev);
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
	ret = neighbour_add(rarps->tha, rarph->ha_len, rarps->tpa, rarph->pa_len,
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
	if ((skb->nh.raw - skb->mac.raw) + ARPG_HEADER_SIZE(rarph) > skb->len) {
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
