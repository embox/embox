/**
 * @file arp.c
 *
 * @brief This module implements the Address Resolution Protocol ARP (RFC 826),
 * which is used to convert IP addresses into a low-level hardware address.
 * @date 11.03.2009
 * @author Anton Bondarev
 */
#include <string.h>
#include <common.h>
#include <in.h>
#include <kernel/module.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <net/net.h>
#include <net/inetdevice.h>
#include <net/etherdevice.h>
#include <net/net_pack_manager.h>
#include <net/arp.h>
#include <net/ip.h>

ARP_ENTITY arp_table[ARP_CACHE_SIZE];

#define ARP_TABLE_SIZE array_len(arp_table)

static struct packet_type arp_packet_type = {
        .type = ETH_P_ARP,
        .func = arp_rcv,
};

void __init arp_init() {
        dev_add_pack(&arp_packet_type);
}

static inline int find_entity(in_device_t *in_dev, in_addr_t dst_addr) {
	int i;
	for (i = 0; i < ARP_TABLE_SIZE; i++) {
		if(arp_table[i].is_busy &&
		  (arp_table[i].pw_addr == dst_addr) &&
		  (in_dev == arp_table[i].if_handler)) {
			return i;
		}
	}
	return -1;
}

/**
 * this function add entry in arp table if can
 * @param iт_dev (handler of inet_dev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return number of entry in table if success else -1
 */
int arp_add_entity(in_device_t *in_dev, in_addr_t ipaddr, unsigned char *macaddr) {
	int i;
	if (-1 != (i = find_entity(in_dev, ipaddr))) {
		return i;
	}
	for (i = 0; i < ARP_TABLE_SIZE; i++) {
		if(0 == arp_table[i].is_busy) {
			arp_table[i].is_busy = 1;
			arp_table[i].if_handler = in_dev;
			arp_table[i].pw_addr = ipaddr;
			memcpy(arp_table[i].hw_addr, macaddr, ETH_ALEN);
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
int arp_delete_entity(in_device_t *in_dev, in_addr_t ipaddr, unsigned char *macaddr) {
	int i;
	for (i = 0; i < ARP_TABLE_SIZE; i++) {
		if( arp_table[i].pw_addr == ipaddr ||
        	    0 == memcmp(arp_table[i].hw_addr, macaddr, ETH_ALEN) ||
        	    in_dev == arp_table[i].if_handler) {
			arp_table[i].is_busy = 0;
    		}
	}
	return -1;
}

sk_buff_t *arp_create(int type, int ptype, in_addr_t dest_ip,
                           net_device_t *dev, in_addr_t src_ip,
			const unsigned char *dest_hw, const unsigned char *src_hw,
			const unsigned char *target_hw) {
	sk_buff_t *pack;
	struct arphdr *arp;
	if (NULL == dev ||
	    NULL == (pack = alloc_skb(0x3b, 0))) {
		return NULL;
	}

	pack->mac.raw = pack->data;
	pack->nh.raw = pack->mac.raw + ETH_HEADER_SIZE;
	arp = pack->nh.arph;

	pack->dev  = dev;
	pack->protocol = htons(ptype);
	if (src_hw == NULL) {
	        src_hw = dev->dev_addr;
	}
	if (dest_hw == NULL) {
	        dest_hw = dev->broadcast;
	}

	/*
	 * Fill the device header for the ARP frame
	 */
	if (dev_hard_header(pack, dev, ptype, (void*)dest_hw, (void*)src_hw, pack->len) < 0) {
		kfree_skb(pack);
		return NULL;
	}

	/*
	 * Fill out the arp protocol part.
	 */
	arp->htype = htons(ARPHRD_ETHER);
	arp->ptype = htons(ETH_P_IP);
	arp->hlen = dev->addr_len;
	arp->plen = IPV4_ADDR_LENGTH;
	arp->oper = htons(type);
	memcpy (arp->sha, src_hw, ETH_ALEN);
	memcpy (arp->tha, dest_hw, ETH_ALEN);
	arp->spa = src_ip;
	arp->tpa = dest_ip;

	return pack;
}

void arp_send(int type, int ptype, in_addr_t dest_ip,
                    struct net_device *dev, in_addr_t src_ip,
                    const unsigned char *dest_hw,
                    const unsigned char *src_hw, const unsigned char *th) {
	struct sk_buff *pack;

	/*
	 * No arp on this interface.
	 */
	if (dev->flags & IFF_NOARP) {
		return;
	}
	pack = arp_create(type, ptype, dest_ip, dev, src_ip,
	                        dest_hw, src_hw, th);
	if (pack == NULL) {
	        return;
	}
	arp_xmit(pack);
}

/**
 * resolve ip address and rebuild net_packet
 * @param pack pointer to net_packet struct
 * @param dst_addr IP address
 * @return pointer to net_packet struct if success else NULL
 */
int arp_find(unsigned char *haddr, sk_buff_t *pack) {
	net_device_t *dev = pack->dev;
	iphdr_t *ip = pack->nh.iph;
	pack->mac.raw = pack->data;
	int i;
	if (ip->daddr != INADDR_BROADCAST) {
		return 1;
	}
	if(-1 != (i = find_entity(NULL, ip->daddr))) {
		memcpy (pack->mac.ethh->h_dest, arp_table[i].hw_addr, ETH_ALEN);
		return 0;
	}

	arp_send(ARPOP_REQUEST, ETH_P_ARP, ip->daddr, dev,
			    ip->saddr, NULL, dev->dev_addr, NULL);

	//TODO delete this after processes will be added to monitor
	usleep(500);
	if (-1 != (i = find_entity(NULL, ip->daddr))) {
		memcpy (pack->mac.ethh->h_dest, arp_table[i].hw_addr, ETH_ALEN);
		return 0;
	}
	return 1;
}

/**
 * receive ARP response, update ARP table
 */
static int received_resp(sk_buff_t *pack) {
	arphdr_t *arp = pack->nh.arph;

	//TODO need add function for getting ip addr
#if 0
	if (inet_dev_get_ipaddr(pack->ifdev) != arp->tpa) {
		return -1;
	}
#endif
	return 0;
}

/**
 * receive ARP request, send ARP response
 */
static int received_req(sk_buff_t *pack) {
	arphdr_t *arp = pack->nh.arph;
	arp_send(ARPOP_REPLY, ETH_P_ARP, arp->spa, pack->dev,
	                        arp->tpa, pack->mac.ethh->h_source, pack->dev->dev_addr, NULL);
	return 0;
}

/**
 * Process an arp request.
 */
static int arp_process(sk_buff_t *pack) {
	int ret;
	struct net_device *dev = pack->dev;
	struct in_device *in_dev = in_dev_get(dev);
	arphdr_t *arp = pack->nh.arph;

	if (ipv4_is_loopback(arp->tpa) || ipv4_is_multicast(arp->tpa)) {
		return 0;
	}

	switch(arp->oper) {
	case htons(ARPOP_REPLY):
		ret = received_resp(pack);
		break;
	case htons(ARPOP_REQUEST):
		ret = received_req(pack);
		break;
        default:
                ret = 0;
        }
        /* add record into arp_table */
        arp_add_entity(in_dev, arp->spa, arp->sha);
        return ret;
}

int arp_rcv(sk_buff_t *pack, net_device_t *dev,
                   packet_type_t *pt, net_device_t *orig_dev) {
	arphdr_t *arp = pack->nh.arph;
	if (arp->hlen != dev->addr_len ||
	    dev->flags & IFF_NOARP ||
	    pack->pkt_type == PACKET_OTHERHOST ||
	    pack->pkt_type == PACKET_LOOPBACK ||
	    arp->plen != 4) {
	        kfree_skb(pack);
	        return 0;
	}

	return arp_process(pack);
}

void arp_xmit(sk_buff_t *skb) {
	dev_queue_xmit(skb);
}
