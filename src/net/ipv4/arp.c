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
#include <net/inetdevice.h>
#include <net/arp.h>
#include <net/ip.h>
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
#if 0
 *  2.3.2.2 (ARP Packet Queue):
 *    SHOULD save at least one packet from each "conversation" with an
 *      unresolved IP address.
#endif
 */


EMBOX_NET_PACK(ETH_P_ARP, arp_rcv, arp_init);

arp_entity_t arp_tables[ARP_CACHE_SIZE];

#define arp_tables_SIZE ARP_CACHE_SIZE

#if 0
static LIST_HEAD(arp_q);
#endif

#if 0
/* Queue an IP packet, while waiting for the ARP reply packet. */
void arp_queue(sk_buff_t *skb) {
	printf("\n\nTo queue %p \n\n", (void *)skb);
	skb->tries = ARP_MAX_TRIES;
	list_add((struct list_head *) skb, &arp_q);
}
#endif

#if 0
/* This will try to retransmit everything on the queue. */
static void arp_send_q(void) {
	struct list_head *skb_h;
	struct sk_buff *skb;
	net_device_t *dev;
	const struct net_device_ops *ops;
	net_device_stats_t *stats;

	list_for_each(skb_h, (struct list_head *)&arp_q) {
		skb = (struct sk_buff *) skb_h;
		dev = skb->dev;
		ops = dev->netdev_ops;
		stats = ops->ndo_get_stats(dev);
		skb->tries--;
		if (skb->tries == 0) {
			list_del(skb_h);
			kfree_skb(skb);
			stats->tx_err++;
			break; /* continue; */
		}
		if (-1 != dev->header_ops->rebuild(skb)) {
			if (-1 == ops->ndo_start_xmit(skb, dev)) {
				stats->tx_err++;
				continue;
			}
			/* update statistic */
			stats->tx_packets++;
			stats->tx_bytes += skb->len;
			list_del(skb_h);
			kfree_skb(skb);
			break; /* remove */
		}
	}
}
#endif

/**
 * Check if there are entries that are too old and remove them.
 */
static void arp_check_expire(sys_tmr_ptr timer, void *param) {
	size_t i;

/* id = ARP_TIMER_ID (MUST) so without checking */
#if 0
	// XXX seems that altering timers from tick handler crashes the system. -- Eldar
	close_timer(ARP_TIMER_ID);
#endif
	for (i = 0; i < ARP_CACHE_SIZE; i++) {
		if ((arp_tables[i].flags == ATF_COM) && (arp_tables[i].state == 1)) {
			arp_tables[i].ctime += ARP_CHECK_INTERVAL;
			if (arp_tables[i].ctime >= ARP_TIMEOUT) {
				arp_tables[i].state = 0;
			}
		}
	}
#if 0
	set_timer(ARP_TIMER_ID, ARP_CHECK_INTERVAL, arp_check_expire);
	arp_send_q();
#endif
}

static int arp_init(void) {
	if (set_timer(NULL, ARP_CHECK_INTERVAL, arp_check_expire, NULL) == 0) {
		return -1;
	}
	return 0;
}

int arp_lookup(in_device_t *in_dev, in_addr_t dst_addr) {
	size_t i;

	for (i = 0; i < arp_tables_SIZE; i++) {
		if ((arp_tables[i].state == 1) && (arp_tables[i].pw_addr == dst_addr)
				&& (arp_tables[i].if_handler == in_dev)) {
			return i;
		}
	}
	return -1;
}

/**
 * this function add entry in arp table (rewrite) if can
 * @param in_dev (handler of inet_dev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return number of entry in table if success else -1
 */
int arp_add_entity(in_device_t *in_dev, in_addr_t ipaddr,
		unsigned char *macaddr, unsigned int flags) {
	size_t i;

	for (i = 0; i < arp_tables_SIZE; i++) {
		if ((arp_tables[i].state == 0) /* if empty */
				|| ((arp_tables[i].pw_addr == ipaddr) /* or exist */
						&& (arp_tables[i].if_handler == in_dev))) {
			arp_tables[i].if_handler = in_dev;
			arp_tables[i].pw_addr = ipaddr;
			arp_tables[i].ctime = 0;
			arp_tables[i].state = 1;
			arp_tables[i].flags = flags;
			memcpy(arp_tables[i].hw_addr, macaddr, ETH_ALEN);
			return i;
		}
	}
	return -1;
}

/**
 * this function delete entry from arp table if can
 * @param in_dev (handler of inet_dev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return number of entry in table if success else -1
 */
int arp_delete_entity(in_device_t *in_dev, in_addr_t ipaddr,
		unsigned char *macaddr) {
	size_t i;

	for (i = 0; i < arp_tables_SIZE; i++) {
		if ((arp_tables[i].pw_addr == ipaddr)
				&& (memcmp(arp_tables[i].hw_addr, macaddr, ETH_ALEN) == 0)
				&& (in_dev == arp_tables[i].if_handler)) {
			arp_tables[i].state = 0;
			return i;
		}
	}
	return -1;
}

sk_buff_t *arp_create(int type, int ptype, in_addr_t dest_ip,
		net_device_t *dev, in_addr_t src_ip, const unsigned char *dest_hw,
		const unsigned char *src_hw, const unsigned char *target_hw) {
	sk_buff_t *skb;
	struct arphdr *arp;
	if (NULL == dev || NULL == (skb = alloc_skb(ETH_HEADER_SIZE
			+ ARP_HEADER_SIZE, 0))) {
		return NULL;
	}

	skb->mac.raw = skb->data;
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;
	arp = skb->nh.arph;

	skb->dev = dev;
	skb->protocol = htons(ptype);
	if (src_hw == NULL) {
		src_hw = dev->dev_addr;
	}
	if (dest_hw == NULL) {
		dest_hw = dev->broadcast;
	}
	/*
	 * Fill the device header for the ARP frame
	 */
	if (dev_hard_header(skb, dev, ptype, (void*) dest_hw, (void*) src_hw,
			skb->len) < 0) {
		kfree_skb(skb);
		return NULL;
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
	memcpy(arp->ar_tha, dest_hw, ETH_ALEN);
	arp->ar_sip = src_ip;
	arp->ar_tip = dest_ip;

	return skb;
}

static int arp_xmit(sk_buff_t *);

int arp_send(int type, int ptype, in_addr_t dest_ip, struct net_device *dev,
		in_addr_t src_ip, const unsigned char *dest_hw,
		const unsigned char *src_hw, const unsigned char *th) {
	struct sk_buff *skb;

	/*
	 * No arp on this interface.
	 */
	if (dev->flags & IFF_NOARP) {
		return -1;
	}
	skb = arp_create(type, ptype, dest_ip, dev, src_ip, dest_hw, src_hw, th);
	if (skb == NULL) {
		return -1;
	}
	return arp_xmit(skb);
}

int arp_resolve(sk_buff_t *pack) {
	size_t i;
	net_device_t *dev;
	iphdr_t *ip;

	ip = pack->nh.iph;
	pack->mac.raw = pack->data;
	if (ip->daddr == INADDR_LOOPBACK) {
		memset(pack->mac.ethh->h_dest, 0x00, ETH_ALEN);
		return 0;
	}
	if (ip->daddr == INADDR_BROADCAST) {
		memset(pack->mac.ethh->h_dest, 0xFF, ETH_ALEN);
		return 0;
	}
	dev = pack->dev;
	if (-1 != (i = arp_lookup(in_dev_get(dev), ip->daddr))) {
		memcpy(pack->mac.ethh->h_dest, arp_tables[i].hw_addr, ETH_ALEN);
		return 0;
	}
	/* send arp request  */
	arp_send(ARPOP_REQUEST, ETH_P_ARP, ip->daddr, dev, ip->saddr, NULL,
			dev->dev_addr, NULL);
	return -1;
}

/**
 * receive ARP response, update ARP table
 */
static int received_resp(sk_buff_t *pack) {
	arphdr_t *arp = pack->nh.arph;

	/*TODO need add function for getting ip addr*/
	/* add record into arp_tables */
	arp_add_entity(in_dev_get(pack->dev), arp->ar_sip, arp->ar_sha, ATF_COM);
	return 0;
}

/**
 * receive ARP request, send ARP response
 */
static int received_req(sk_buff_t *skb) {
	arphdr_t *arp = skb->nh.arph;
	return arp_send(ARPOP_REPLY, ETH_P_ARP, arp->ar_sip, skb->dev, arp->ar_tip,
			skb->mac.ethh->h_source, skb->dev->dev_addr, NULL);
}

/**
 * Process an arp request.
 */
static int arp_process(sk_buff_t *skb) {
	int ret = 0;
	struct net_device *dev = skb->dev;
	arphdr_t *arp = skb->nh.arph;

	if (ipv4_is_loopback(arp->ar_tip) || ipv4_is_multicast(arp->ar_tip)
			|| (arp->ar_tip != in_dev_get(dev)->ifa_address)) {
		kfree_skb(skb);
		return 0;
	}
	switch (ntohs(arp->ar_op)) {
		case ARPOP_REPLY:
			ret = received_resp(skb);
			break;
		case ARPOP_REQUEST:
			ret = received_req(skb);
			break;
	}
	kfree_skb(skb);
	return ret;
}

int arp_rcv(sk_buff_t *skb, net_device_t *dev, packet_type_t *pt,
		net_device_t *orig_dev) {
	arphdr_t *arp = skb->nh.arph;

	if ((arp->ar_hln != dev->addr_len)
			|| (dev->flags & IFF_NOARP)
			|| (skb->pkt_type == PACKET_OTHERHOST)
			|| (skb->pkt_type == PACKET_LOOPBACK) // why?
			|| (arp->ar_pln != 4)) {
		kfree_skb(skb);
		return NET_RX_SUCCESS;
	}
	return arp_process(skb);
}

static int arp_xmit(sk_buff_t *skb) {
	return dev_queue_xmit(skb);
}
