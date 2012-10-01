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
#include <net/if_ether.h> /* for ETH_A_LEN */
#include <assert.h>
#include <net/skbuff.h>
#include <net/ip.h>
#include <net/inetdevice.h>
#include <embox/net/pack.h>

EMBOX_NET_PACK(ETH_P_RARP, rarp_rcv, rarp_init);

static int rarp_build(struct sk_buff *skb, unsigned short oper,
		in_addr_t dest_ip, 	in_addr_t src_ip, const unsigned char *dest_hw,
		const unsigned char *src_hw, struct net_device *dev) {
	int ret;
	struct arphdr *rarph;

	assert(skb != NULL);
	assert(dev != NULL);

	assert(dev->addr_len == ETH_ALEN); /* FIXME */

	skb->dev = dev;
	skb->protocol = ETH_P_RARP;
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;

	src_hw = ((src_hw != NULL) ? src_hw : &dev->dev_addr[0]);
	dest_hw = ((dest_hw != NULL) ? dest_hw : &dev->broadcast[0]);

	assert(dev->header_ops != NULL);
	assert(dev->header_ops->create != NULL);
	ret = dev->header_ops->create(skb, dev, skb->protocol,
			(void *)dest_hw, (void *)src_hw, skb->len);
	if (ret != ENOERR) {
		return ret;
	}

	rarph = skb->nh.rarph;
	assert(rarph != NULL);
	rarph->ar_hrd = htons(dev->type);
	rarph->ar_pro = htons(ETH_P_IP);
	rarph->ar_hln = dev->addr_len;
	rarph->ar_pln = IP_ADDR_LEN;
	rarph->ar_op = htons(oper);
	memcpy(&rarph->ar_sha[0], src_hw, rarph->ar_hln);
	rarph->ar_sip = src_ip;
	memcpy(&rarph->ar_tha[0], dest_hw, rarph->ar_hln);
	rarph->ar_tip = dest_ip;

	return ENOERR;
}

static int rarp_xmit(struct sk_buff *skb) {
	return dev_queue_xmit(skb);
}

int rarp_send(unsigned short oper, in_addr_t dest_ip, in_addr_t src_ip,
		const unsigned char *dest_hw, const unsigned char *src_hw,
		struct net_device *dev) {
	int ret;
	struct sk_buff *skb;

	assert(dev != NULL);

	if (dev->flags & IFF_NOARP) {
		return -EINVAL;
	}

	skb = skb_alloc(ETH_HEADER_SIZE + ARP_HEADER_SIZE);
	if (skb == NULL) {
		return -ENOMEM;
	}

	ret = rarp_build(skb, oper, dest_ip, src_ip, dest_hw, src_hw, dev);
	if (ret != ENOERR) {
		skb_free(skb);
		return ret;
	}

	return rarp_xmit(skb);
}

static int rarp_hnd_ip(struct arphdr *rarph, struct sk_buff *skb,
		struct net_device *dev) {
	int ret;
	in_addr_t source_ip, dest_ip;
	unsigned char dest_hw[MAX_ADDR_LEN];

	assert(rarph != NULL);
	assert(dev != NULL);

	if (rarph->ar_pln != IPV4_ADDR_LENGTH) {
		skb_free(skb);
		return -1; /* error: bad protocol address length */
	}

	switch (ntohs(rarph->ar_op)) {
	default:
		skb_free(skb);
		return -1; /* error: bad operation type */
	case ARPOP_RREQUEST: /* TODO server part */
		assert(in_dev_get(dev) != NULL);
		source_ip = in_dev_get(dev)->ifa_address;
		dest_ip = 0xAABBCCDD;
		memcpy(&dest_hw[0], &rarph->ar_sha[0], rarph->ar_hln);
		/* build reply */
		ret = rarp_build(skb, ARPOP_RREPLY, dest_ip, source_ip,
				&dest_hw[0], &dev->dev_addr[0], dev);
		if (ret != ENOERR) {
			skb_free(skb);
			return ret;
		}
		/* and send */
		return rarp_xmit(skb);
	case ARPOP_RREPLY:
		ret = neighbour_add(&rarph->ar_sha[0], rarph->ar_hln,
				(const unsigned char *)&rarph->ar_sip, rarph->ar_pln,
				dev, NEIGHBOUR_FLAG_PERMANENT);
		skb_free(skb);
		return ret;
	}
}

static int rarp_process(struct sk_buff *skb, struct net_device *dev) {
	struct arphdr *rarph;

	assert(skb != NULL);
	assert(dev != NULL);

	rarph = skb->nh.rarph;
	assert(rarph != NULL);

	if ((rarph->ar_hrd != htons(ARPHRD_ETHER))
			|| (rarph->ar_hln != dev->addr_len)) {
		skb_free(skb);
		return -1; /* TODO */
	}

	if (memcmp(&rarph->ar_tha[0], &dev->dev_addr[0],
				dev->addr_len) != 0) {
		skb_free(skb);
		return -1; /* error: not for us */
	}

	switch (ntohs(rarph->ar_pro)) {
	default:
		skb_free(skb);
		return -1; /* error: bad protocol id */
	case ETH_P_IP:
		return rarp_hnd_ip(rarph, skb, dev);
	}
}

int rarp_rcv(struct sk_buff *skb, struct net_device *dev,
		struct packet_type *pt, struct net_device *orig_dev) {
	assert(skb != NULL);
	assert(dev != NULL);

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

	skb_free(skb);
	return NET_RX_DROP;
}

static int rarp_init(void) {
	return ENOERR;
}

