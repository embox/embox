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

#include <arpa/inet.h>
#include <net/util/macaddr.h>
#include <util/log.h>

#include <framework/mod/options.h>
#define LOG_LEVEL OPTION_GET(STRING, log_level)

static void log_arp_hnd_request(const struct arphdr *rarph,
		uint8_t *dst_paddr, uint8_t *dst_haddr);
static void log_arp_hnd_reply(const struct arphdr *rarph,
		const struct arpbody *rarpb);

EMBOX_NET_PACK(ETH_P_ARP, arp_rcv);

static int arp_send(struct sk_buff *skb, struct net_device *dev,
		uint16_t pro, uint8_t pln, uint16_t op, const void *sha,
		const void *spa, const void *tha, const void *tpa) {
	int ret;
	struct net_header_info hdr_info;

	assert(dev != NULL);
	if (dev->flags & IFF_NOARP) {
		log_error("arp isn't supported by device %s", dev->name);
		return -ENOSUPP;
	}

	assert(skb != NULL);
	assert(pln != 0);
	assert(sha != NULL);
	assert(spa != NULL);
	assert(tha != NULL);
	assert(tpa != NULL);

	/* build device specific header */
	/* TODO move to l0 level */
	hdr_info.type = ETH_P_ARP;
	hdr_info.src_hw = sha;
	hdr_info.dst_hw = tha;

	assert(dev->ops != NULL);
	assert(dev->ops->build_hdr != NULL);
	ret = dev->ops->build_hdr(skb, &hdr_info);
	if (ret != 0) {
		log_error("can't build device header");
		return ret;
	}

	arp_build(arp_hdr(skb), dev->type, pro, dev->addr_len, pln,
			op, sha, spa, tha, tpa);


	net_tx_direct(skb);

	return 0;
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

	if ((arph->ar_pro != htons(ETH_P_IP))
			|| (arph->ar_pln != sizeof in_dev->ifa_address)) {
		log_error("only IPv4 is supported");
		return -ENOSUPP; /* FIXME error: only IPv4 is supported */
	}

	if (0 != arp_update_neighbour(arph, arpb, dev)) {
		log_error("can't update neighbour");
	}

	if (0 != memcmp(arpb->ar_tpa, &in_dev->ifa_address,
				arph->ar_pln)) {
		log_error("not for us");
		return -EINVAL;
	}

	/* save src protocol address */
	memcpy(&src_paddr[0], arpb->ar_tpa, arph->ar_pln);

	/* save dest hardware/protocol addresses */
	memcpy(&dst_haddr[0], arpb->ar_sha, arph->ar_hln);
	memcpy(&dst_paddr[0], arpb->ar_spa, arph->ar_pln);

	if (NULL == skb_declone(skb)) {
		log_error("no memory");
		return -ENOMEM;
	}

	log_arp_hnd_request(arph, &dst_paddr[0], &dst_haddr[0]);

	return arp_send(skb, dev, ntohs(arph->ar_pro), arph->ar_pln,
			ARP_OP_REPLY, &dev->dev_addr[0], &src_paddr[0],
			&dst_haddr[0], &dst_paddr[0]);
}

static int arp_hnd_reply(const struct arphdr *arph,
		const struct arpbody *arpb, struct sk_buff *skb,
		struct net_device *dev) {
	int ret;

	ret = arp_update_neighbour(arph, arpb, dev);
	if (ret != 0) {
		log_error("can't update neighbour");
		return ret;
	}

	log_arp_hnd_reply(arph, arpb);

	return 0;
}

static void log_arp_hnd_request(const struct arphdr *arph,
		uint8_t *dst_paddr, uint8_t *dst_haddr) {
	log_debug("send reply with ");
	if (arph->ar_pro == ntohs(ETH_P_IP)) {
		struct in_addr in;
		assert(arph->ar_pln == sizeof in);
		memcpy(&in, dst_paddr, sizeof in);
		log_debug("%s", inet_ntoa(in));
	} else {
		log_debug("unknown(%x)", htons(arph->ar_pro));
	}

	if (arph->ar_hrd == ntohs(ARP_HRD_ETHERNET)) {
		assert(arph->ar_hln == ETH_ALEN);
		log_debug("[" MACADDR_FMT "]",
			MACADDR_FMT_ARG(dst_haddr));
	} else {
		log_debug("[unknown(%x)]", htons(arph->ar_hrd));
	}
}

static void log_arp_hnd_reply(const struct arphdr *arph,
		const struct arpbody *arpb) {
	log_debug("receive reply with ");
	if (arph->ar_pro == ntohs(ETH_P_IP)) {
		struct in_addr in;
		assert(arph->ar_pln == sizeof in);
		memcpy(&in, arpb->ar_spa, sizeof in);
		log_debug("%s", inet_ntoa(in));
	} else {
		log_debug("unknown(%x)", htons(arph->ar_pro));
	}

	if (arph->ar_hrd == ntohs(ARP_HRD_ETHERNET)) {
		assert(arph->ar_hln == ETH_ALEN);
		log_debug("[" MACADDR_FMT "]",
			MACADDR_FMT_ARG(arpb->ar_sha));
	} else {
		log_debug("[unknown(%x)]", htons(arph->ar_hrd));
	}
}

static int arp_rcv(struct sk_buff *skb, struct net_device *dev) {
	struct arphdr *arph;
	struct arpbody arpb;
	int ret = 0;

	assert(skb != NULL);
	assert(dev != NULL);

	if (dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		log_error("arp isn't supported by device %s", &dev->name[0]);
		return 0;
	}

	arph = arp_hdr(skb);

	if (dev->hdr_len + ARP_HEADER_SIZE(arph) > skb->len) {
		log_error("bad packet length");
		skb_free(skb);
		return 0;
	}

	if ((arph->ar_hrd != htons(dev->type))
			|| (arph->ar_hln != dev->addr_len)) {
		log_error("invalid hardware type or address length");
		skb_free(skb);
		return 0;
	}

	arp_make_body(arph, &arpb);

	switch (arph->ar_op) {
	default:
		skb_free(skb);
		log_error("bad operation code");
		return 0;
	case htons(ARP_OP_REQUEST):
		ret = arp_hnd_request(arph, &arpb, skb, dev);
		/* If there are no errors, skb was passed to net stack,
		 * so we don't need to free it */
		if (ret != 0) {
			skb_free(skb);
		}

		return ret;
	case htons(ARP_OP_REPLY):
		ret = arp_hnd_reply(arph, &arpb, skb, dev);

		skb_free(skb);

		return ret;
	}
}

int arp_discover(struct net_device *dev, uint16_t pro,
		uint8_t pln, const void *spa, const void *tpa) {
	size_t size;
	int ret;
	struct sk_buff *skb;

	if (dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		return -ENOSUPP;
	}

	size = dev->hdr_len + ARP_CALC_HEADER_SIZE(dev->addr_len, pln);
	if (size > dev->mtu) {
		return -EMSGSIZE;
	}

	skb = skb_alloc(size);
	if (skb == NULL) {
		return -ENOMEM;
	}

	skb->dev = dev;
	skb->nh.raw = skb->mac.raw + dev->hdr_len;

	ret = arp_send(skb, dev, pro, pln, ARP_OP_REQUEST,
			&dev->dev_addr[0], spa, &dev->broadcast[0], tpa);

	/* If there are no errors, skb was passed to net stack,
	 * so we don't need to free it */
	if (ret != 0) {
		skb_free(skb);
	}
	return ret;
}
