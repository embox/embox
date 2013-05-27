/**
 * @file
 * @brief Ethernet protocol options
 *
 * @date 04.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <linux/etherdevice.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_ether.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <kernel/printk.h>
#include <stdio.h>
#include <string.h>
#include <util/array.h>

#include <net/arp.h> /* FIXME */

/**
 * Create the Ethernet header
 * @param pack buffer to alter
 * @param type Ethernet type field
 * @param daddr destination address (NULL leave destination address)
 * @param saddr source address (NULL use device source address)
 */
static int ethernet_create_hdr(struct sk_buff *skb, unsigned short type,
		const void *daddr, const void *saddr) {
	struct ethhdr *ethh;
	const struct net_device *dev;

	if (skb == NULL) {
		return -EINVAL;
	}

	dev = skb->dev;
	assert(dev != NULL);

	daddr = daddr != NULL ? daddr : &dev->broadcast[0];
	saddr = saddr != NULL ? saddr : &dev->dev_addr[0];

	ethh = skb->mac.ethh;
	assert(ethh != NULL);

	ethh->h_proto = htons(type);

	if (dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		memset(ethh->h_dest, 0, ETH_ALEN);
		memset(ethh->h_source, 0, ETH_ALEN);
	}
	else {
		memcpy(ethh->h_dest, daddr, ETH_ALEN);
		memcpy(ethh->h_source, saddr, ETH_ALEN);
	}

	return 0;
}

/**
 * Rebuild the Ethernet MAC header.
 * @param pack socket buffer to update
 */
static int ethernet_rebuild_hdr(struct sk_buff *skb) {
	int ret;
	struct ethhdr *ethh;

	if (skb == NULL) {
		return -EINVAL;
	}

	ethh = skb->mac.ethh;
	assert(ethh != NULL);

	ethh->h_proto = htons(skb->protocol);

	switch (skb->protocol) {
	case ETH_P_LOOP:
		/* Fill out source and destonation MAC addresses */
		memset(ethh->h_source, 0, ETH_ALEN);
		memset(ethh->h_dest, 0, ETH_ALEN);
		break;
	case ETH_P_IP:
		/* Fill out source MAC address */
		memcpy(ethh->h_source, &skb->dev->dev_addr[0], ETH_ALEN);
		/* Fill out destonation MAC address */
		ret = arp_resolve(skb);
		if (ret != 0) {
			return ret;
		}
		break;
	case ETH_P_IPV6:
		break;
	}

	return 0;
}

static int ethernet_parse_hdr(struct sk_buff *skb) {
	skb->protocol = ntohs(skb->mac.ethh->h_proto);

	if (skb->len <= ETH_HEADER_SIZE) {
		return -EINVAL;
	}

	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;

	return 0;
}

static int ethernet_check_addr(const void *addr) {
	if (addr == NULL) {
		return -EINVAL;
	}

	if (!is_valid_ether_addr(addr)) {
		return -EINVAL;
	}

	return 0;
}

static int ethernet_check_mtu(int mtu) {
	if ((mtu < ETH_ZLEN) || (mtu > ETH_FRAME_LEN)) {
		return -EINVAL;
	}

	return 0;
}

const struct net_device_ops ethernet_ops = {
	.create_hdr = &ethernet_create_hdr,
	.rebuild_hdr = &ethernet_rebuild_hdr,
	.parse_hdr = &ethernet_parse_hdr,
	.check_addr = &ethernet_check_addr,
	.check_mtu = &ethernet_check_mtu
};

int ethernet_setup(struct net_device *dev) {
	static unsigned int eth_id = 0;
	int ret;
	char name_fmt[IFNAMSIZ];

	if (dev == NULL) {
		return -EINVAL;
	}

	strcpy(&name_fmt[0], &dev->name[0]);
	ret = snprintf(&dev->name[0], ARRAY_SIZE(dev->name),
			&name_fmt[0], eth_id);
	if (ret < 0) {
		return -EIO;
	}
	else if (ret >= ARRAY_SIZE(dev->name)) {
		return -ENOMEM;
	}
	++eth_id;

	memset(&dev->broadcast[0], 0xFF, ETH_ALEN);
	dev->type = ARPG_HRD_ETHERNET;
	dev->addr_len = ETH_ALEN;
	dev->flags = IFF_BROADCAST | IFF_MULTICAST;
	dev->mtu = ETH_FRAME_LEN;
	dev->tx_queue_len = 1000;
	dev->ops = &ethernet_ops;

	return 0;
}
