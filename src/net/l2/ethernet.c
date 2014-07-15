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
#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/netdevice.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <util/array.h>
#include <kernel/printk.h>

static int ethernet_build_hdr(struct sk_buff *skb,
		const struct net_header_info *hdr_info) {
	struct ethhdr *ethh;

	assert(skb != NULL);
	assert(hdr_info != NULL);

	ethh = eth_hdr(skb);
	ethh->h_proto = htons(hdr_info->type);

	assert(skb->dev != NULL);
	if (skb->dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		memset(ethh->h_dest, 0, ETH_ALEN);
		memset(ethh->h_source, 0, ETH_ALEN);
	}
	else {
		assert(hdr_info->src_hw != NULL);
		assert(hdr_info->dst_hw != NULL);
		memcpy(ethh->h_source, hdr_info->src_hw, ETH_ALEN);
		memcpy(ethh->h_dest, hdr_info->dst_hw, ETH_ALEN);
	}

	return 0;
}

static int ethernet_parse_hdr(struct sk_buff *skb,
		struct net_header_info *out_hdr_info) {
	const struct ethhdr *ethh;

	assert(skb != NULL);
	assert(out_hdr_info != NULL);

	ethh = eth_hdr(skb);

	memset(out_hdr_info, 0, sizeof *out_hdr_info);
	out_hdr_info->type = ntohs(ethh->h_proto);
	out_hdr_info->src_hw = ethh->h_source;
	out_hdr_info->dst_hw = ethh->h_dest;

	return 0;
}

static int ethernet_check_mtu(int mtu) {
	return (mtu >= ETH_ZLEN) && (mtu <= ETH_FRAME_LEN);
}

const struct net_device_ops ethernet_ops = {
	.build_hdr = ethernet_build_hdr,
	.parse_hdr = ethernet_parse_hdr,
	.check_addr = is_valid_ether_addr,
	.check_mtu = ethernet_check_mtu
};

static int ethernet_setup(struct net_device *dev) {
	static unsigned int eth_id = 0;
	int ret;
	char name_fmt[IFNAMSIZ];

	assert(dev);

	strcpy(name_fmt, dev->name);
	ret = snprintf(dev->name, ARRAY_SIZE(dev->name), name_fmt, eth_id);
	if (ret < 0)
		return -EIO;
	if (ret >= ARRAY_SIZE(dev->name))
		return -ENOMEM;

	++eth_id;

	memset(&dev->broadcast[0], 0xff, ETH_ALEN);
	dev->type = ARP_HRD_ETHERNET;
	dev->hdr_len = ETH_HEADER_SIZE;
	dev->addr_len = ETH_ALEN;
	dev->flags = IFF_BROADCAST | IFF_MULTICAST;
	dev->mtu = ETH_FRAME_LEN;
	dev->ops = &ethernet_ops;

	return 0;
}

struct net_device * etherdev_alloc(size_t priv_size) {
	return netdev_alloc("eth%u", &ethernet_setup, priv_size);
}

void etherdev_free(struct net_device *dev) {
	netdev_free(dev);
}
