/**
 * @file
 * @brief This module implements the Address Resolution Protocol (ARP),
 * which is used to convert IP addresses into a low-level hardware address.
 * @details RFC 826
 *
 * @date 11.05.11
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <string.h>
#include <kernel/timer.h>
#include <net/icmp.h>
#include <net/inetdevice.h>
#include <net/arp.h>
#include <net/ip.h>
#include <embox/net/pack.h>
#include <errno.h>
#include <net/neighbour.h>
#include <time.h>

#include <net/arp_queue.h>

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

static int arp_header(sk_buff_t *skb, int type, int ptype,
		in_addr_t dest_ip, net_device_t *dev, in_addr_t src_ip,
		const unsigned char *dest_hw, const unsigned char *src_hw,
		const unsigned char *target_hw) {
	struct arphdr *arp;

	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;
	arp = skb->nh.arph;

	skb->dev = dev;
	skb->protocol = ptype;
	if (src_hw == NULL) {
		src_hw = dev->dev_addr;
	}
	if (dest_hw == NULL) {
		dest_hw = dev->broadcast;
	}

	/*
	 * Fill the device header for the ARP frame
	 */
	if (dev_hard_header(skb, dev, ptype, (void *)dest_hw,
			(void *)src_hw, skb->len) < 0) {
		return -1;
	}

	/*
	 * Fill out the arp protocol part.
	 */
	arp->ar_hrd = htons(ARPHRD_ETHER);
	arp->ar_pro = htons(ETH_P_IP);
	arp->ar_hln = dev->addr_len;
	arp->ar_pln = IPV4_ADDR_LENGTH;
	arp->ar_op = htons(type);
	memcpy(arp->ar_sha, src_hw, ETH_ALEN);
	arp->ar_sip = src_ip;
	memcpy(arp->ar_tha, dest_hw, ETH_ALEN);
	arp->ar_tip = dest_ip;

	return 0;
}

static int arp_xmit(sk_buff_t *skb) {
	return dev_queue_xmit(skb);
}

int arp_send(int type, int ptype, in_addr_t dest_ip,
		struct net_device *dev, in_addr_t src_ip,
		const unsigned char *dest_hw, const unsigned char *src_hw,
		const unsigned char *th) {
	struct sk_buff *skb;

	if (!dev) {
		return -1;
	}
	/*
	 * No arp on this interface.
	 */
	if (dev->flags & IFF_NOARP) {
		return -1;
	}
	if (!(skb = alloc_skb(ETH_HEADER_SIZE + ARP_HEADER_SIZE, 0))) {
		return -1;
	}
	if (arp_header(skb, type, ptype, dest_ip, dev, src_ip, dest_hw, src_hw, th)) {
		kfree_skb(skb);
		return -1;
	}
	return arp_xmit(skb);
}

int arp_resolve(sk_buff_t *pack) {
	uint8_t *hw_addr;
	net_device_t *dev;
	iphdr_t *ip;

	ip = pack->nh.iph;
	/* loopback */
	if (ipv4_is_loopback(ip->daddr) || (ip->daddr == ip->saddr)) {
		memset(pack->mac.ethh->h_dest, 0x00, ETH_ALEN);
		return 0;
	}
	/* broadcast */
	if (ip->daddr == htonl(INADDR_BROADCAST)) {
		memset(pack->mac.ethh->h_dest, 0xFF, ETH_ALEN);
		return 0;
	}
	dev = pack->dev;
#if 0
	/* our machine on our device? */
	if(ip->daddr == inet_dev_get_ipaddr(in_dev_get(dev))){
		memcpy(pack->mac.ethh->h_dest, dev->dev_addr, ETH_ALEN);
		return 0;
	}
#endif
	/* someone on the net */
	if ((hw_addr = neighbour_lookup(in_dev_get(dev), ip->daddr)) != NULL) {
		memcpy(pack->mac.ethh->h_dest, hw_addr, ETH_ALEN);
		return 0;
	}

	/* send arp request and add packet in list of deferred packets */
	arp_queue_add(pack);
	arp_send(ARPOP_REQUEST, ETH_P_ARP, ip->daddr, dev, ip->saddr, NULL,
			dev->dev_addr, NULL);

	return -1;
}

/**
 * receive ARP response, update ARP table
 */
static int received_resp(sk_buff_t *pack) {
	arphdr_t *arp;

	arp = pack->nh.arph;
	/*TODO need add function for getting ip addr*/
	/* add record into arp_tables */
	return neighbour_add(in_dev_get(pack->dev), arp->ar_sip, arp->ar_sha, ATF_COM);
}

/**
 * receive ARP request, send ARP response
 */
static int received_req(sk_buff_t *skb) {
	arphdr_t *arp;

	arp = skb->nh.arph;
	if (arp_header(skb, ARPOP_REPLY, ETH_P_ARP, arp->ar_sip, skb->dev, arp->ar_tip, arp->ar_sha, skb->dev->dev_addr, NULL)) {
		kfree_skb(skb);
		return -1;
	}
	return arp_xmit(skb);
}

/**
 * Process an arp request.
 */
static int arp_process(sk_buff_t *skb) {
	int res;
	arphdr_t *arp;

	arp = skb->nh.arph;
	if (ipv4_is_loopback(arp->ar_tip) || ipv4_is_multicast(arp->ar_tip)) {
		kfree_skb(skb);
		return 0;
	}
	if (arp->ar_tip != in_dev_get(skb->dev)->ifa_address) {
		if (arp->ar_tip == arp->ar_sip) { /* RFC 3927 - ARP Announcement */
			neighbour_add(in_dev_get(skb->dev), arp->ar_sip, arp->ar_sha, ATF_COM);
		}
		kfree_skb(skb);
		return -1;
	}
	switch (ntohs(arp->ar_op)) {
		case ARPOP_REPLY:
			res = received_resp(skb);
			arp_queue_process(skb);
			kfree_skb(skb);
			return res;
		case ARPOP_REQUEST:
			received_resp(skb);
			return received_req(skb);
	}
	kfree_skb(skb);
	return -1;
}

int arp_rcv(sk_buff_t *skb, net_device_t *dev, packet_type_t *pt,
		net_device_t *orig_dev) {
	arphdr_t *arp;

	arp = skb->nh.arph;
	if ((arp->ar_hln != dev->addr_len) || (dev->flags & IFF_NOARP)
			|| (skb->pkt_type == PACKET_OTHERHOST)
			|| (skb->pkt_type == PACKET_LOOPBACK)
			|| (arp->ar_pln != 4)) {
		kfree_skb(skb);
		return NET_RX_SUCCESS;
	}
	return (arp_process(skb) ? NET_RX_DROP : NET_RX_SUCCESS);
}

static int arp_init(void) {
	return ENOERR;
}
