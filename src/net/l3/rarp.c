/**
 * @file
 * @brief A Reverse Address Resolution Protocol
 * @details RFC 903
 *
 * @date 29.09.12
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <embox/net/pack.h>
#include <framework/mod/options.h>
#include <net/if.h>
#include <net/inetdevice.h>
#include <net/l0/net_tx.h>
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/l3/rarp.h>
#include <net/lib/arp.h>
#include <net/neighbour.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/util/macaddr.h>
#include <util/log.h>
#include <util/math.h>

// Forward declarations
static void log_rarp_hnd_request(const struct arphdr *rarph, uint8_t *dst_paddr,
    uint8_t *dst_haddr);
static void log_rarp_hnd_reply(const struct arphdr *rarph,
    const struct arpbody *rarpb);

EMBOX_NET_PACK(ETH_P_RARP, rarp_rcv);

static int rarp_xmit(struct sk_buff *skb) {
	assert(skb != NULL);
	assert(skb->dev != NULL);
	if (skb->dev->flags & IFF_NOARP) {
		log_error("rarp_xmit: rarp doesn't supported by device %s\n",
		    &skb->dev->name[0]);
		skb_free(skb);
		return 0; /* error: rarp doesn't supported by device */
	}

	/* fall through to dev layer */
	return net_tx_direct(skb);
}

static int rarp_send(struct sk_buff *skb, struct net_device *dev, uint16_t pro,
    uint8_t pln, uint16_t op, const void *sha, const void *spa, const void *tha,
    const void *tpa, const void *tar) {
	int ret;
	size_t size;
	struct net_header_info hdr_info;

	assert(skb != NULL);
	assert(dev != NULL);
	assert(pln != 0);
	assert(sha != NULL);
	assert(spa != NULL);
	assert(tha != NULL);
	assert(tpa != NULL);
	assert(tar != NULL);

	size = dev->hdr_len + ARP_CALC_HEADER_SIZE(dev->addr_len, pln);
	if (size > min(dev->mtu, skb_max_size())) {
		log_error("rarp_send: hdr size %zu is too big (max %zu)\n", size,
		    min(dev->mtu, skb_max_size()));
		if (skb) {
			skb_free(skb); /* TODO */
		}
		return -EMSGSIZE; /* error: hdr size is too big */
	}

	skb = skb_realloc(size, skb);
	if (skb == NULL) {
		log_error("rarp_send: no memory\n");
		return -ENOMEM; /* error: no memory */
	}

	skb->dev = dev;
	skb->nh.raw = skb->mac.raw + dev->hdr_len;

	/* build device specific header */
	/* TODO move to l0 level */
	hdr_info.type = ETH_P_RARP;
	hdr_info.src_hw = sha;
	hdr_info.dst_hw = tar;
	assert(dev->ops != NULL);
	assert(dev->ops->build_hdr != NULL);
	ret = dev->ops->build_hdr(skb, &hdr_info);
	if (ret != 0) {
		log_error("rarp_send: can't build device header\n");
		skb_free(skb);
		return ret;
	}

	/* build RARP header */
	arp_build(arp_hdr(skb), dev->type, pro, dev->addr_len, pln, op, sha, spa,
	    tha, tpa);

	/* and send */
	return rarp_xmit(skb);
}

static int rarp_hnd_request(const struct arphdr *rarph,
    const struct arpbody *rarpb, struct sk_buff *skb, struct net_device *dev) {
	int ret;
	uint8_t src_paddr[MAX_ADDR_LEN];
	uint8_t dst_haddr[MAX_ADDR_LEN], dst_paddr[MAX_ADDR_LEN];
	uint8_t tar_haddr[MAX_ADDR_LEN];
	struct in_device *in_dev;

	in_dev = inetdev_get_by_dev(dev);
	assert(in_dev != NULL);

	/* check protocol capabilities */
	if ((rarph->ar_pro != htons(ETH_P_IP))
	    || (rarph->ar_pln != sizeof in_dev->ifa_address)) {
		log_error("rarp_hnd_request: only IPv4 is supported\n");
		skb_free(skb);
		return 0; /* FIXME error: only IPv4 is supported */
	}

	/* get source protocol address */
	memcpy(&src_paddr[0], &in_dev->ifa_address, rarph->ar_pln);

	/* get dest addresses */
	memcpy(&dst_haddr[0], rarpb->ar_tha, rarph->ar_hln);
	ret = neighbour_get_paddr(ntohs(rarph->ar_hrd), &dst_haddr[0], dev,
	    ntohs(rarph->ar_pro), rarph->ar_pln, &dst_paddr[0]);
	if (ret != 0) {
		log_error("rarp_hnd_request: paddr not found in neighbour\n");
		skb_free(skb);
		return ret != -ENOENT ? ret : 0;
	}

	/* get target hardware address */
	memcpy(&tar_haddr[0], rarpb->ar_sha, rarph->ar_hln);

	/* declone sk_buff */
	if (NULL == skb_declone(skb)) {
		log_error("rarp_hnd_request: no memory\n");
		skb_free(skb);
		return -ENOMEM;
	}

	if (log_level_self() == LOG_DEBUG) {
		log_rarp_hnd_request(rarph, &dst_paddr[0], &dst_haddr[0]);
	}

	/* send reply */
	return rarp_send(skb, dev, ntohs(rarph->ar_pro), rarph->ar_pln,
	    RARP_OP_REPLY, &dev->dev_addr[0], &src_paddr[0], &dst_haddr[0],
	    &dst_paddr[0], &tar_haddr[0]);
}

static int rarp_hnd_reply(const struct arphdr *rarph,
    const struct arpbody *rarpb, struct sk_buff *skb, struct net_device *dev) {
	int ret;

	assert(rarph != NULL);
	assert(rarpb != NULL);

	/* save destination hardware and protocol addresses */
	ret = neighbour_add(ntohs(rarph->ar_pro), rarpb->ar_tpa, rarph->ar_pln, dev,
	    ntohs(rarph->ar_hrd), rarpb->ar_tha, rarph->ar_hln, 0);
	if (ret != 0) {
		log_error("rarp_hnd_reply: can't update neighbour\n");
		skb_free(skb);
		return ret;
	}

	if (log_level_self() == LOG_DEBUG) {
		log_rarp_hnd_reply(rarph, rarpb);
	}

	/* free sk_buff */
	skb_free(skb);

	return 0;
}

static void log_rarp_hnd_request(const struct arphdr *rarph, uint8_t *dst_paddr,
    uint8_t *dst_haddr) {
	log_debug("rarp_hnd_request: send reply with ");
	if (rarph->ar_pro == ntohs(ETH_P_IP)) {
		struct in_addr in;
		assert(rarph->ar_pln == sizeof in);
		memcpy(&in, dst_paddr, sizeof in);
		log_debug("%s", inet_ntoa(in));
	}
	else {
		log_debug("unknown(%x)", htons(rarph->ar_pro));
	}
	if (rarph->ar_hrd == ntohs(ARP_HRD_ETHERNET)) {
		assert(rarph->ar_hln == ETH_ALEN);
		log_debug("[" MACADDR_FMT "]", MACADDR_FMT_ARG(dst_haddr));
	}
	else {
		log_debug("[unknown(%x)]", htons(rarph->ar_hrd));
	}
	log_debug("\n");
}

static void log_rarp_hnd_reply(const struct arphdr *rarph,
    const struct arpbody *rarpb) {
	log_debug("rarp_hnd_reply: receive reply with ");
	if (rarph->ar_pro == ntohs(ETH_P_IP)) {
		struct in_addr in;
		assert(rarph->ar_pln == sizeof in);
		memcpy(&in, rarpb->ar_tpa, sizeof in);
		log_debug("%s", inet_ntoa(in));
	}
	else {
		log_debug("unknown(%x)", htons(rarph->ar_pro));
	}
	if (rarph->ar_hrd == ntohs(ARP_HRD_ETHERNET)) {
		assert(rarph->ar_hln == ETH_ALEN);
		log_debug("[" MACADDR_FMT "]", MACADDR_FMT_ARG(rarpb->ar_tha));
	}
	else {
		log_debug("[unknown(%x)]", htons(rarph->ar_hrd));
	}
	log_debug("\n");
}

static int rarp_rcv(struct sk_buff *skb, struct net_device *dev) {
	struct arphdr *rarph;
	struct arpbody rarpb;

	assert(skb != NULL);
	assert(dev != NULL);

	/* check device flags */
	if (dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		return 0; /* error: rarp doesn't supported by device */
	}

	rarph = arp_hdr(skb);

	/* check hardware and protocol address lengths */
	if (dev->hdr_len + ARP_HEADER_SIZE(rarph) > skb->len) {
		skb_free(skb);
		return 0; /* error: bad packet */
	}

	/* check device capabilities */
	if ((rarph->ar_hrd != htons(dev->type))
	    || (rarph->ar_hln != dev->addr_len)) {
		skb_free(skb);
		return 0; /* error: invalid hardware address info */
	}

	/* build rarp body */
	arp_make_body(rarph, &rarpb);

	/* process the packet by the operation code */
	switch (rarph->ar_op) {
	default:
		skb_free(skb);
		return 0; /* error: bad operation type */
	case htons(RARP_OP_REQUEST):
		/* handling request */
		return rarp_hnd_request(rarph, &rarpb, skb, dev);
	case htons(RARP_OP_REPLY):
		/* handling reply */
		return rarp_hnd_reply(rarph, &rarpb, skb, dev);
	}
}
