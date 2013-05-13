/**
 * @file
 * @brief Ethernet protocol options.
 *
 * @date 04.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include <net/arp.h>
#include <net/etherdevice.h>
#include <net/if.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <kernel/printk.h>

/**
 * Create the Ethernet header
 * @param pack buffer to alter
 * @param dev source device
 * @param type Ethernet type field
 * @param daddr destination address (NULL leave destination address)
 * @param saddr source address (NULL use device source address)
 */
static int eth_create_header(struct sk_buff *skb, struct net_device *dev,
		unsigned short type, const void *daddr, const void *saddr) {
	struct ethhdr *eth;

	assert(skb != NULL);
	assert(dev != NULL);

	eth = eth_hdr(skb);
	assert(eth != NULL);

	eth->h_proto = htons(type);

	if (dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		memset(eth->h_dest, 0, ETH_ALEN);
		memset(eth->h_source, 0, ETH_ALEN);
	} else {
		memcpy(eth->h_dest,
				daddr != NULL ? daddr : &dev->broadcast[0],
				ETH_ALEN);
		memcpy(eth->h_source,
				saddr != NULL ? saddr : &dev->dev_addr[0],
				ETH_ALEN);
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
	assert(eth != NULL);

	eth->h_proto = htons(skb->protocol);

	switch (skb->protocol) {
	default:
		printk("%s: unable to resolve type %X addresses.\n", dev->name,
				(int)skb->protocol);
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
		if (ret != 0) {
			return ret;
		}
		break;
	}

	return 0;
}

static const struct header_ops eth_header_ops = {
	.create        = eth_create_header,
	.rebuild       = eth_rebuild_header,
};

const struct header_ops * eth_get_header_ops(void) {
	return &eth_header_ops;
}
