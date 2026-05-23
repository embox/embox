/**
 * @file
 * @brief
 *
 * @date 19.06.11
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */
#include <util/log.h>

#include <assert.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stddef.h>

#include <net/l0/net_crypt.h>
#include <net/l0/net_tx.h>
#include <net/neighbour.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/socket/packet.h>
#include <net/l2/ethernet.h>
#include "net/l3/arp.h"

extern int netif_tx(struct net_device *dev,  struct sk_buff *skb);

int net_tx_direct(struct sk_buff *skb) {
	struct net_device *dev;

	assert(skb != NULL);

	dev = skb->dev;
	assert(dev != NULL);

	if (!(dev->flags & IFF_UP)) {
		log_error("device is down");
		skb_free(skb);
		return -ENETDOWN;
	}

	log_debug("%p len %zu type %#.6hx", skb, skb->len, ntohs(skb->mac.ethh->h_proto));

	/*
	 * http://www.linuxfoundation.org/collaborate/workgroups/networking/kernel_flow#Transmission_path
	 * Search for:
	 * dev_hard_start_xmit() calls the hard_start_xmit virtual method for the net_device.
	 * But first, it calls dev_queue_xmit_nit(), which checks if a packet handler has been registered
	 * for the ETH_P_ALL protocol. This is used for tcpdump.
	 */
	sock_packet_add(skb, htons(ETH_P_ALL));

	skb = net_encrypt(skb);
	if (skb == NULL) {
		return 0;
	}

	netif_tx(dev, skb);

	return 0;
}

int net_tx(struct sk_buff *skb, struct net_header_info *hdr_info) {
	int ret;
	unsigned char dst_haddr[MAX_ADDR_LEN];
	struct net_device *dev;

	assert(skb != NULL);

	dev = skb->dev;
	assert(dev != NULL);

	if (dev->type == ARP_HRD_NONE)
	{
		goto send;
	}

	if (dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		eth_hdr(skb)->h_proto = htons(hdr_info->type);
		goto send;
	}

	if (hdr_info->dst_p == NULL) {
		/* it's loopback/local or broadcast address? */
		hdr_info->dst_hw = &dev->broadcast[0];
	} else {
		ret = neighbour_resolve(hdr_info->type,
				hdr_info->dst_p, hdr_info->p_len, dev, skb,
				sizeof(dst_haddr), &dst_haddr[0]);
		if (ret != 0) {
			return 0; /* we just wait arp resolving */
		}

		hdr_info->dst_hw = &dst_haddr[0];
	}

	/* try to build */
	assert(dev->ops != NULL);
	assert(dev->ops->build_hdr != NULL);
	ret = dev->ops->build_hdr(skb, hdr_info);
	if (ret != 0) {
		return ret;
	}

send:
	net_tx_direct(skb);

	return 0;
}
