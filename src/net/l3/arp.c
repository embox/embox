/**
 * @file
 * @brief An Ethernet Address Resolution Protocol
 * @details RFC 826
 *
 * @date 11.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <embox/net/pack.h>
#include <errno.h>
#include <net/if.h>
#include <net/inetdevice.h>
#include <net/l0/net_tx.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/lib/arp.h>
#include <net/neighbour.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <util/math.h>

EMBOX_NET_PACK(ETH_P_ARP, arp_rcv);

#define ARP_DEBUG 0
#if ARP_DEBUG
#include <arpa/inet.h>
#include <kernel/printk.h>
#include <net/util/macaddr.h>
#define DBG(x) x
#else
#define DBG(x)
#endif

static int arp_xmit(struct sk_buff *skb) {
	assert(skb != NULL);
	assert(skb->dev != NULL);
	if (skb->dev->flags & IFF_NOARP) {
		DBG(printk("arp_xmit: arp doesn't supported by device %s\n",
					&skb->dev->name[0]));
		skb_free(skb);
		return 0; /* error: arp doesn't supported by device */
	}

	/* fall through to dev layer */
	return net_tx(skb, NULL);
}

static int arp_send(struct sk_buff *skb, struct net_device *dev,
		uint16_t pro, uint8_t pln, uint16_t op, const void *sha,
		const void *spa, const void *tha, const void *tpa) {
	int ret;
	size_t size;
	struct net_header_info hdr_info;

	assert(dev != NULL);
	assert(pln != 0);
	assert(sha != NULL);
	assert(spa != NULL);
	assert(tha != NULL);
	assert(tpa != NULL);

	size = dev->hdr_len + ARP_CALC_HEADER_SIZE(dev->addr_len, pln);
	if (size > min(dev->mtu, skb_max_size())) {
		DBG(printk("arp_send: hdr size %zu is too big (max %zu)\n",
					size, min(dev->mtu, skb_max_size())));
		if (skb) skb_free(skb); /* TODO */
		return -EMSGSIZE; /* error: hdr size is too big */
	}

	skb = skb_realloc(size, skb);
	if (skb == NULL) {
		DBG(printk("arp_send: no memory\n"));
		return -ENOMEM; /* error: no memory */
	}

	skb->dev = dev;
	skb->nh.raw = skb->mac.raw + dev->hdr_len;

	/* build device specific header */
	/* TODO move to l0 level */
	hdr_info.type = ETH_P_ARP;
	hdr_info.src_hw = sha;
	hdr_info.dst_hw = tha;
	assert(dev->ops != NULL);
	assert(dev->ops->build_hdr != NULL);
	ret = dev->ops->build_hdr(skb, &hdr_info);
	if (ret != 0) {
		DBG(printk("arp_send: can't build device header\n"));
		skb_free(skb);
		return ret; /* error: can't build device specific header */
	}

	/* build ARP header */
	arp_build(arp_hdr(skb), dev->type, pro, dev->addr_len, pln,
			op, sha, spa, tha, tpa);

	/* and send */
	return arp_xmit(skb);
}

static int arp_update_neighbour(const struct arphdr *arph,
		const struct arpbody *arpb, struct net_device *dev) {
	assert(arph != NULL);
	assert(arpb != NULL);

	return neighbour_add(ntohs(arph->ar_pro), arpb->ar_spa,
			arph->ar_pln, dev, ntohs(arph->ar_hrd), arpb->ar_sha,
			arph->ar_hln, 0);
}

static int arp_hnd_request(const struct arphdr *arph,
		const struct arpbody *arpb, struct sk_buff *skb,
		struct net_device *dev) {
	uint8_t src_paddr[MAX_ADDR_LEN];
	uint8_t dst_haddr[MAX_ADDR_LEN], dst_paddr[MAX_ADDR_LEN];
	struct in_device *in_dev;

	in_dev = inetdev_get_by_dev(dev);
	assert(in_dev != NULL);

	/* check protocol capabilities */
	if ((arph->ar_pro != htons(ETH_P_IP))
			|| (arph->ar_pln != sizeof in_dev->ifa_address)) {
		DBG(printk("arp_hnd_request: only IPv4 is supported\n"));
		skb_free(skb);
		return 0; /* FIXME error: only IPv4 is supported */
	}

	/* update translation table */
	if (0 != arp_update_neighbour(arph, arpb, dev)) {
		DBG(printk("arp_hnd_request: can't update neighbour\n"));
	}

	/* check recipient */
	if (0 != memcmp(arpb->ar_tpa, &in_dev->ifa_address,
				arph->ar_pln)) {
		DBG(printk("arp_hnd_request: not for us\n"));
		skb_free(skb);
		return 0; /* error: not for us */
	}

	/* save src protocol address */
	memcpy(&src_paddr[0], arpb->ar_tpa, arph->ar_pln);

	/* save dest hardware/protocol addresses */
	memcpy(&dst_haddr[0], arpb->ar_sha, arph->ar_hln);
	memcpy(&dst_paddr[0], arpb->ar_spa, arph->ar_pln);

	/* declone sk_buff */
	if (NULL == skb_declone(skb)) {
		DBG(printk("arp_hnd_request: no memory\n"));
		skb_free(skb);
		return -ENOMEM;
	}

	DBG({
		printk("arp_hnd_request: send reply with ");
		if (arph->ar_pro == ntohs(ETH_P_IP)) {
			struct in_addr in;
			assert(arph->ar_pln == sizeof in);
			memcpy(&in, dst_paddr, sizeof in);
			printk("%s", inet_ntoa(in));
		}
		else {
			printk("unknown(%x)", htons(arph->ar_pro));
		}
		if (arph->ar_hrd == ntohs(ARP_HRD_ETHERNET)) {
			assert(arph->ar_hln == ETH_ALEN);
			printk("[" MACADDR_FMT "]",
				MACADDR_FMT_ARG(dst_haddr));
		}
		else {
			printk("[unknown(%x)]", htons(arph->ar_hrd));
		}
		printk("\n");
	});

	/* send reply */
	return arp_send(skb, dev, ntohs(arph->ar_pro), arph->ar_pln,
			ARP_OP_REPLY, &dev->dev_addr[0], &src_paddr[0],
			&dst_haddr[0], &dst_paddr[0]);
}

static int arp_hnd_reply(const struct arphdr *arph,
		const struct arpbody *arpb, struct sk_buff *skb,
		struct net_device *dev) {
	int ret;

	/* update translation table */
	ret = arp_update_neighbour(arph, arpb, dev);
	if (ret != 0) {
		DBG(printk("arp_hnd_reply: can't update neighbour\n"));
		skb_free(skb);
		return ret;
	}

	DBG({
		printk("arp_hnd_reply: receive reply with ");
		if (arph->ar_pro == ntohs(ETH_P_IP)) {
			struct in_addr in;
			assert(arph->ar_pln == sizeof in);
			memcpy(&in, arpb->ar_spa, sizeof in);
			printk("%s", inet_ntoa(in));
		}
		else {
			printk("unknown(%x)", htons(arph->ar_pro));
		}
		if (arph->ar_hrd == ntohs(ARP_HRD_ETHERNET)) {
			assert(arph->ar_hln == ETH_ALEN);
			printk("[" MACADDR_FMT "]",
				MACADDR_FMT_ARG(arpb->ar_sha));
		}
		else {
			printk("[unknown(%x)]", htons(arph->ar_hrd));
		}
		printk("\n");
	});

	/* free sk_buff */
	skb_free(skb);

	return 0;
}

static int arp_rcv(struct sk_buff *skb, struct net_device *dev) {
	struct arphdr *arph;
	struct arpbody arpb;

	assert(skb != NULL);
	assert(dev != NULL);

	/* check device flags */
	if (dev->flags & IFF_NOARP) {
		DBG(printk("arp_rcv: aro doesn't supported by device %s\n",
					&dev->name[0]));
		return 0; /* error: arp doesn't supported by device */
	}

	arph = arp_hdr(skb);

	/* check hardware and protocol address lengths */
	if (dev->hdr_len + ARP_HEADER_SIZE(arph) > skb->len) {
		DBG(printk("arp_rcv: bad packet length\n"));
		skb_free(skb);
		return 0; /* error: bad packet */
	}


	/* check device capabilities */
	if ((arph->ar_hrd != htons(dev->type))
			|| (arph->ar_hln != dev->addr_len)) {
		DBG(printk("arp_rcv: invalid hardware type or address length\n"));
		skb_free(skb);
		return 0; /* error: invalid hardware address info */
	}

	/* build arp body */
	arp_make_body(arph, &arpb);

	/* process the packet by the operation code */
	switch (arph->ar_op) {
	default:
		skb_free(skb);
		DBG(printk("arp_rcv: bad operation code\n"));
		return 0; /* error: bad operation code */
	case htons(ARP_OP_REQUEST):
		/* handling request */
		return arp_hnd_request(arph, &arpb, skb, dev);
	case htons(ARP_OP_REPLY):
		/* handling reply */
		return arp_hnd_reply(arph, &arpb, skb, dev);
	}
}

int arp_discover(struct net_device *dev, uint16_t pro,
		uint8_t pln, const void *spa, const void *tpa) {
	return arp_send(NULL, dev, pro, pln, ARP_OP_REQUEST,
			&dev->dev_addr[0], spa, &dev->broadcast[0], tpa);
}
