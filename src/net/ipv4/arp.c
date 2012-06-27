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
#include <assert.h>
//#include <kernel/thread/sched.h>

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

static int arp_header(struct sk_buff *skb, struct net_device *dev,
		int oper, int protocol, in_addr_t dest_ip, in_addr_t src_ip,
		const unsigned char *dest_hw, const unsigned char *src_hw,
		const unsigned char *target_hw) {
	int res;
	struct arphdr *arph;

	assert((skb != NULL) && (dev != NULL));

	skb->dev = dev;
	skb->protocol = protocol;
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;

	src_hw = (src_hw != NULL ? src_hw : dev->dev_addr);
	dest_hw = (dest_hw != NULL ? dest_hw : dev->broadcast);

	/*
	 * Fill the device header for the ARP frame
	 */
	res = dev_hard_header(skb, dev, protocol,
			(void *)dest_hw, (void *)src_hw, skb->len);
	if (res < 0) {
		return res;
	}

	/*
	 * Fill out the arp protocol part.
	 */
	arph = skb->nh.arph;
	assert(arph != NULL);

	arph->ar_hrd = htons(ARPHRD_ETHER);
	arph->ar_pro = htons(ETH_P_IP);
	arph->ar_hln = dev->addr_len;
	arph->ar_pln = IPV4_ADDR_LENGTH;
	arph->ar_op = htons(oper);
	memcpy(arph->ar_sha, src_hw, ETH_ALEN);
	arph->ar_sip = src_ip;
	memcpy(arph->ar_tha, dest_hw, ETH_ALEN);
	arph->ar_tip = dest_ip;

	return ENOERR;
}

static int arp_xmit(struct sk_buff *skb) {
	assert(skb != NULL);

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

	skb = skb_alloc(ETH_HEADER_SIZE + ARP_HEADER_SIZE);
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

int arp_resolve(sk_buff_t *pack) {
	int res;
	uint8_t *hw_addr;
	net_device_t *dev;
	iphdr_t *ip;

	ip = pack->nh.iph;
	/* loopback */
	if (ipv4_is_loopback(ip->daddr) || (ip->daddr == ip->saddr)) {
		memset(pack->mac.ethh->h_dest, 0x00, ETH_ALEN);
		return ENOERR;
	}
	/* broadcast */
	if (ip->daddr == htonl(INADDR_BROADCAST)) {
		memset(pack->mac.ethh->h_dest, 0xFF, ETH_ALEN);
		return ENOERR;
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
	hw_addr = neighbour_lookup(in_dev_get(dev), ip->daddr);
	if (hw_addr != NULL) {
		memcpy(pack->mac.ethh->h_dest, hw_addr, ETH_ALEN);
		return ENOERR;
	}

	/* send arp request and add packet in list of deferred packets */
	res = arp_queue_add(pack);
	if (res < 0) {
		return res;
	}

	res = arp_send(ARPOP_REQUEST, ETH_P_ARP, dev, ip->daddr, ip->saddr, NULL,
			dev->dev_addr, NULL);
	if (res < 0) {
		return res;
	}

#if 0 // TODO it's a temporary solution
	/* FIXME:
	 * Really it's waiting until event_fire have not been called
	 * and third argument is SCHED_TIMEOUT_INFINITE but in this case
	 * we have a problem:
	 * sched_sleep is called after that arp-reply was received,
	 * so we wait until the NEXT reply is received.
	 */
	sched_sleep(&pack->sk->sock_is_ready, 500/*SCHED_TIMEOUT_INFINITE*/);

	/* FIXME:
	 * if there return ENOERR then package will be send two times:
	 * 1. by arp_queue_process
	 * 2. in the course of execution this thread
	 */
	return (sock_is_ready(pack->sk) ? ENOERR : -ENOENT);
#else
//	return -ENOENT;
#endif

	res = arp_queue_wait_resolve(pack);
	if (res < 0) {
		return res;
	}

	/* try again */
	hw_addr = neighbour_lookup(in_dev_get(dev), ip->daddr);
	assert(hw_addr != NULL);

	memcpy(pack->mac.ethh->h_dest, hw_addr, ETH_ALEN);

	return ENOERR;
}

/**
 * receive ARP response, update ARP table
 */
static int received_resp(struct sk_buff *skb, struct net_device *dev) {
	struct arphdr *arph;

	assert((skb != NULL) && (dev != NULL));

	arph = skb->nh.arph;
	assert(arph != NULL);

	/*TODO need add function for getting ip addr*/
	/* add record into arp_tables */
	return neighbour_add(in_dev_get(dev), arph->ar_sip, arph->ar_sha, ATF_COM);
}

/**
 * receive ARP request, send ARP response
 */
static int received_req(struct sk_buff *skb, struct net_device *dev) {
	int res;
	struct arphdr *arph;

	assert((skb != NULL) && (dev != NULL));

	arph = skb->nh.arph;
	assert(arph != NULL);

	res = arp_header(skb, dev, ARPOP_REPLY, ETH_P_ARP, arph->ar_sip,
			arph->ar_tip, arph->ar_sha, dev->dev_addr, NULL);
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
	struct arphdr *arph;
	struct in_device *in_dev;

	assert((skb != NULL) && (dev != NULL));

	arph = skb->nh.arph;
	assert(arph != NULL);

	if (ipv4_is_loopback(arph->ar_tip) || ipv4_is_multicast(arph->ar_tip)) {
		skb_free(skb);
		return 0;
	}

	in_dev = in_dev_get(dev);
	assert(in_dev != NULL);

	if (arph->ar_tip != in_dev->ifa_address) {
		if (arph->ar_tip == arph->ar_sip) { /* RFC 3927 - ARP Announcement */
			neighbour_add(in_dev, arph->ar_sip, arph->ar_sha, ATF_COM);
		}
		skb_free(skb);
		return -1;
	}

	switch (ntohs(arph->ar_op)) {
		case ARPOP_REPLY:
			res = received_resp(skb, dev);
			arp_queue_process(skb);
			skb_free(skb);
			return res;
		case ARPOP_REQUEST:
			received_resp(skb, dev);
			return received_req(skb, dev);
	}

	skb_free(skb);
	return -1;
}

int arp_rcv(struct sk_buff *skb, struct net_device *dev,
		struct packet_type *pt, struct net_device *orig_dev) {
	struct arphdr *arph;

	assert((skb != NULL) && (dev != NULL));

	arph = skb->nh.arph;
	assert(arph != NULL);

	switch (eth_packet_type(skb)) {
	case PACKET_HOST:
	case PACKET_BROADCAST:
	case PACKET_MULTICAST:
		if (!(dev->flags & IFF_NOARP)
				&& (arph->ar_hrd == htons(ARPHRD_ETHER))
				&& (arph->ar_pro == htons(ETH_P_IP))
				&& (arph->ar_hln == dev->addr_len)
				&& (arph->ar_pln == IPV4_ADDR_LENGTH)) {
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
