/**
 * @file
 * @brief Ethernet protocol options.
 *
 * @date 4.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <err.h>
#include <errno.h>
#include <net/arp.h>
#include <net/etherdevice.h>
#include <net/if.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <string.h>
#include <assert.h>
#include <compiler.h>

/**
 * Create the Ethernet header
 * @param pack buffer to alter
 * @param dev source device
 * @param type Ethernet type field
 * @param daddr destination address (NULL leave destination address)
 * @param saddr source address (NULL use device source address)
 * @paramlen packet length (<= pack->len)
 */
static int eth_create_header(struct sk_buff *skb, struct net_device *dev,
		unsigned short type, void *daddr, void *saddr, unsigned len) {
	struct ethhdr *eth;

	assert(skb != NULL);
	assert(dev != NULL);
	assert(saddr != NULL);

	eth = eth_hdr(skb);
	eth->h_proto = htons(type);

	/*  Set the source hardware address. */
	if (saddr == NULL) {
		saddr = dev->dev_addr;
	}
	memcpy(eth->h_source, saddr, ETH_ALEN);

	if (dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		/* Anyway, the loopback-device should never use this function... */
		memset(eth->h_dest, 0, ETH_ALEN);
	} else {
		assert(daddr != NULL);

		memcpy(eth->h_dest, daddr, ETH_ALEN);
	}

	return ENOERR;
}

/**
 * Rebuild the Ethernet MAC header.
 * @param pack socket buffer to update
 * TODO which fields should be affected by this?
 */
static int eth_rebuild_header(struct sk_buff *skb) {
	int ret;
	struct ethhdr *eth;
	struct net_device *dev;

	assert(skb != NULL);

	dev = skb->dev;
	assert(dev != NULL);

	eth = eth_hdr(skb);
	eth->h_proto = htons(skb->protocol);

	switch (skb->protocol) {
	default:
		LOG_ERROR("%s: unable to resolve type %X addresses.\n",
					dev->name, (int)skb->protocol);
		return -EINVAL;
	case ETH_P_LOOP:
		/* Fill out source and destonation MAC addresses */
		memset(eth->h_source, 0x00, ETH_ALEN);
		memset(eth->h_dest, 0x00, ETH_ALEN);
		break;
	case ETH_P_IP:
		/* Fill out source MAC address */
		memcpy(eth->h_source, dev->dev_addr, ETH_ALEN);
		/* Fill out destonation MAC address */
		ret = arp_resolve(skb);
		if (ret < 0) {
			return ret;
		}
		break;
	}

	return ENOERR;
}

/**
 * Extract hardware address from packet.
 * @param pack packet to extract header from
 * @param haddr destination buffer
 */
static int eth_header_parse(const sk_buff_t *pack, unsigned char *haddr) {
	assert(pack != NULL);
	assert(haddr != NULL);

	memcpy(haddr, pack->mac.raw, ETH_ALEN);

	return ENOERR;
}

static const struct header_ops eth_header_ops = {
	.create        = eth_create_header,
	.rebuild       = eth_rebuild_header,
	.parse         = eth_header_parse,
};

const struct header_ops * eth_get_header_ops(void) {
	return &eth_header_ops;
}

uint8_t eth_packet_type(struct sk_buff *skb) {
	struct ethhdr *eth;
	struct net_device *dev;

	assert(skb != NULL);

	dev = skb->dev;
	assert(dev != NULL);

	eth = eth_hdr(skb);

	if (dev == inet_get_loopback_dev()) {
		return PACKET_LOOPBACK;
	}

	if (is_multicast_ether_addr(eth->h_dest)) {
		if (!compare_ether_addr(eth->h_dest, dev->broadcast))
			return PACKET_BROADCAST;
		else
			return PACKET_MULTICAST;
	} else {
		if (unlikely(compare_ether_addr(eth->h_dest, dev->dev_addr))) {
			return PACKET_OTHERHOST;
		} else {
			return PACKET_HOST;
		}
	}
}

