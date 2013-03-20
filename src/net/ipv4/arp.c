/**
 * @file
 * @brief This module implements the Address Resolution Protocol (ARP),
 * which is used to convert IP addresses into a low-level hardware address.
 * @details RFC 826
 *
 * @date 11.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <string.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#include <kernel/time/timer.h>

#include <net/icmp.h>
#include <net/inetdevice.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/neighbour.h>
#include <net/route.h>
#include <net/arp_queue.h>
#include <linux/in.h>
#include <net/etherdevice.h>

#include <embox/net/pack.h>


/*
 * FIXME:
 * RFC 3927
 * ARP Announcement && ARP Probe
 */
/* RFC1122 Status:
 *  2.3.2.1 (ARP Cache Validation):
 *    MUST provide mechanism to flush stale cache entries
 *    SHOULD be able to configure cache timeout
 *    MUST throttle ARP retransmits
 *  2.3.2.2 (ARP Packet Queue):
 *    SHOULD save at least one packet from each "conversation" with an
 *      unresolved IP address.
 */

EMBOX_NET_PACK(ETH_P_ARP, arp_rcv, arp_init);

static int arp_header(struct sk_buff *skb, struct net_device *dev,
		int oper, int protocol, in_addr_t dest_ip, in_addr_t src_ip,
		const unsigned char *dest_hw, const unsigned char *src_hw,
		const unsigned char *target_hw) {
	int res;
	struct arpghdr *arph;
	struct arpg_stuff arph_stuff;

	assert((skb != NULL) && (dev != NULL));

	skb->dev = dev;
	skb->protocol = protocol;
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;

	src_hw = (src_hw != NULL ? src_hw : dev->dev_addr);
	dest_hw = (dest_hw != NULL ? dest_hw : dev->broadcast);

	/*
	 * Fill the device header for the ARP frame
	 */
	res = dev->header_ops->create(skb, dev, protocol,
			(void *)dest_hw, (void *)src_hw, skb->len);
	if (res < 0) {
		return res;
	}

	/*
	 * Fill out the arp protocol part.
	 */
	arph = skb->nh.arpgh;
	assert(arph != NULL);
	arph->ha_space = htons(ARPG_HRD_ETHERNET);
	arph->pa_space = htons(ETH_P_IP);
	arph->ha_len = dev->addr_len;
	arph->pa_len = IP_ADDR_LEN;
	arph->oper = htons(oper);
	arpg_make_stuff(arph, &arph_stuff);
	memcpy(&arph_stuff.sha[0], src_hw, ETH_ALEN);
	memcpy(&arph_stuff.spa[0], &src_ip, IP_ADDR_LEN);
	memcpy(&arph_stuff.tha[0], dest_hw, ETH_ALEN);
	memcpy(&arph_stuff.tpa[0], &dest_ip, IP_ADDR_LEN);

	return ENOERR;
}

static int arp_xmit(struct sk_buff *skb) {
	return dev_queue_xmit(skb);
}

int arp_send(int type, int ptype, struct net_device *dev,
		in_addr_t dest_ip, in_addr_t src_ip,
		const unsigned char *dest_hw, const unsigned char *src_hw,
		const unsigned char *th) {
	int res;
	struct sk_buff *skb;

	assert(dev != NULL);

	if (dev->flags & IFF_NOARP) {
		return -EINVAL;
	}

	skb = skb_alloc(ETH_HEADER_SIZE + ARPG_CALC_HDR_SZ(ETH_ALEN, IP_ADDR_LEN));
	if (skb == NULL) {
		return -ENOMEM;
	}

	res = arp_header(skb, dev, type, ptype, dest_ip, src_ip,
			dest_hw, src_hw, th);
	if (res < 0) {
		skb_free(skb);
		return res;
	}

	return arp_xmit(skb);
}

int arp_resolve(struct sk_buff *skb) {
	int ret;
	in_addr_t daddr;

	assert(skb != NULL);

	/* get ip after routing */
	ret = rt_fib_route_ip(skb->nh.iph->daddr, &daddr);
	if (ret != 0) {
		return ret;
	}

	/* loopback */
	if (ipv4_is_loopback(daddr) || (daddr == skb->nh.iph->saddr)) {
		memset(skb->mac.ethh->h_dest, 0x00, ETH_ALEN);
		return ENOERR;
	}

	/* broadcast */
	if (daddr == htonl(INADDR_BROADCAST)) {
		memset(skb->mac.ethh->h_dest, 0xFF, ETH_ALEN);
		return ENOERR;
	}

#if 0
	/* our machine on our device? */
	if (ip->daddr == inetdev_get_addr(inetdev_get_by_dev(dev))){
		memcpy(pack->mac.ethh->h_dest, dev->dev_addr, ETH_ALEN);
		return 0;
	}
#endif

	/* someone on the net */
	ret = neighbour_get_hardware_address((const unsigned char *)&daddr,
			sizeof daddr, skb->dev, sizeof skb->mac.ethh->h_dest,
			skb->mac.ethh->h_dest, NULL);
	if (ret != ENOERR) {
		return ret;
	}

	return ENOERR;
}

/**
 * receive ARP response, update ARP table
 */
static int received_resp(struct sk_buff *skb, struct net_device *dev) {
	struct arpghdr *arph;
	struct arpg_stuff arph_stuff;

	assert((skb != NULL) && (dev != NULL));

	arph = skb->nh.arpgh;
	assert(arph != NULL);

	arpg_make_stuff(arph, &arph_stuff);

	/*TODO need add function for getting ip addr*/
	/* add record into arp_tables */
	return neighbour_add(arph_stuff.sha, arph->ha_len, arph_stuff.spa,
			arph->pa_len, dev, 0);
}

/**
 * receive ARP request, send ARP response
 */
static int received_req(struct sk_buff *skb, struct net_device *dev) {
	int res;
	struct arpghdr *arph;
	struct arpg_stuff arph_stuff;
	unsigned char dest_hw[MAX_ADDR_LEN];

	assert((skb != NULL) && (dev != NULL));

	arph = skb->nh.arpgh;
	assert(arph != NULL);

	arpg_make_stuff(arph, &arph_stuff);

	memcpy(dest_hw, arph_stuff.sha, arph->ha_len); /* save dest hardware address */
	res = arp_header(skb, dev, ARP_OPER_REPLY, ETH_P_ARP, *(in_addr_t *)arph_stuff.spa,
			*(in_addr_t *)arph_stuff.tpa, &dest_hw[0], &dev->dev_addr[0], NULL);
	if (res < 0) {
		skb_free(skb);
		return res;
	}

	return arp_xmit(skb);
}

/**
 * Process an arp request.
 */
static int arp_process(struct sk_buff *skb, struct net_device *dev) {
	int res;
	struct arpghdr *arph;
	struct arpg_stuff arph_stuff;
	struct in_device *in_dev;
	in_addr_t target_ip;

	assert((skb != NULL) && (dev != NULL));

	arph = skb->nh.arpgh;
	assert(arph != NULL);

	arpg_make_stuff(arph, &arph_stuff);
	target_ip = *(in_addr_t *)arph_stuff.tpa;

	if (ipv4_is_loopback(target_ip) || ipv4_is_multicast(target_ip)) {
		skb_free(skb);
		return 0;
	}

	in_dev = inetdev_get_by_dev(dev);
	assert(in_dev != NULL);

	if (target_ip != in_dev->ifa_address) {
		if (target_ip == *(in_addr_t *)arph_stuff.spa) { /* RFC 3927 - ARP Announcement */
			neighbour_add(arph_stuff.sha, arph->ha_len, arph_stuff.spa,
					arph->pa_len, dev, 0);
		}
		skb_free(skb);
		return -1;
	}

	switch (ntohs(arph->oper)) {
	case ARP_OPER_REPLY:
		res = received_resp(skb, dev);
		arp_queue_process(skb);
		skb_free(skb);
		return res;
	case ARP_OPER_REQUEST:
		received_resp(skb, dev);
		return received_req(skb, dev);
	}

	skb_free(skb);
	return -1;
}

int arp_rcv(struct sk_buff *skb, struct net_device *dev,
		struct packet_type *pt, struct net_device *orig_dev) {
	struct arpghdr *arph;

	assert((skb != NULL) && (dev != NULL));

	arph = skb->nh.arpgh;
	assert(arph != NULL);

	switch (eth_packet_type(skb)) {
	case PACKET_HOST:
	case PACKET_BROADCAST:
	case PACKET_MULTICAST:
		if (!(dev->flags & IFF_NOARP)
				&& (arph->ha_space == htons(ARPG_HRD_ETHERNET))
				&& (arph->pa_space == htons(ETH_P_IP))
				&& (arph->ha_len == dev->addr_len)
				&& (arph->pa_len == IP_ADDR_LEN)) {
			return (arp_process(skb, dev) < 0 ? NET_RX_DROP : NET_RX_SUCCESS);
		}
		break;
	}

	skb_free(skb);
	return NET_RX_DROP;
}

static int arp_init(void) {
	return ENOERR;
}
