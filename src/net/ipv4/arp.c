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
#include <lib/inet/netinet/in.h>
#include <kernel/module.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <net/net.h>
#include <net/inetdevice.h>
#include <net/etherdevice.h>
#include <net/net_pack_manager.h>
#include <net/arp.h>

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

sk_buff_t* arp_create(in_device_t *in_dev, in_addr_t dst_addr) {
	sk_buff_t *pack;

	if (NULL == in_dev ||
	    NULL == (pack = alloc_skb((int)pack->len, 0))) {
		return NULL;
	}

	pack->dev  = in_dev->dev;
	pack->mac.raw = pack->data;
	/* mac header */
	memset (pack->mac.ethh->h_dest, 0xFF, ETH_ALEN);
	memcpy (pack->mac.ethh->h_source, pack->dev->dev_addr, ETH_ALEN);
	pack->mac.ethh->h_proto = ETH_P_ARP;

	pack->nh.raw = pack->mac.raw + ETH_HEADER_SIZE;

	/* arp header */
	pack->nh.arph->htype = ARPHRD_ETHER;
	pack->nh.arph->ptype = ETH_P_IP;
	//TODO length hardware and logical type
	pack->nh.arph->hlen = pack->dev->addr_len;
	pack->nh.arph->plen = IPV4_ADDR_LENGTH;
	pack->nh.arph->oper = ARPOP_REQUEST;
	memcpy (pack->nh.arph->sha, pack->dev->dev_addr, ETH_ALEN);
	pack->nh.arph->spa = inet_dev_get_ipaddr(in_dev);
	pack->nh.arph->tpa = dst_addr;

	pack->len = 0x3b;
	pack->protocol = ETH_P_ARP;
	return pack;
}

/**
 * resolve ip address and rebuild net_packet
 * @param pack pointer to net_packet struct
 * @param dst_addr IP address
 * @return pointer to net_packet struct if success else NULL
 */
//int arp_find(unsigned char *haddr, sk_buff_t *pack)
sk_buff_t *arp_find(sk_buff_t *pack, in_addr_t dst_addr) {
	//TODO:
	net_device_t *dev = pack->dev;
	int i;
#if 0
	void *ifdev = pack->ifdev;

	if (NULL == pack || NULL == ifdev) {
		return NULL;
	}
#endif
	if (dst_addr != INADDR_BROADCAST) {
		pack->mac.raw = pack->data;
		memcpy (pack->mac.ethh->h_dest, arp_table[i].hw_addr, ETH_ALEN);
		return pack;
	}
#if 0
	if (-1 != (i = find_entity(ifdev, dst_addr))) {
		pack->mac.raw = pack->data;
		memset (pack->mac.ethh->h_dest, 0xFF, ETH_ALEN);
		return pack;
	}

	/*send mac packet*/
	arp_xmit(arp_create(ifdev, dst_addr));

	//TODO delete this after processes will be added to monitor
	usleep(500);
	if (-1 != (i = find_entity(ifdev, dst_addr))) {
		pack->mac.raw = pack->data;
		memcpy (pack->mac.ethh->h_dest, arp_table[i].hw_addr, ETH_ALEN);
		return pack;
	}
#endif
	return NULL;
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
	sk_buff_t *resp;
	arphdr_t *arp = pack->nh.arph;
	unsigned char mac[18];
	macaddr_print(mac, arp->sha);
	struct in_addr spa;
	spa.s_addr = arp->spa;

	resp = skb_copy(pack, 0);
	resp->dev = pack->dev;
	resp->protocol = pack->protocol;
	memcpy(resp->mac.ethh->h_dest, pack->mac.ethh->h_source, ETH_ALEN);
	memcpy(resp->mac.ethh->h_source, pack->dev->dev_addr, ETH_ALEN);
	resp->nh.arph->oper = ARPOP_REPLY;
	memcpy(resp->nh.arph->sha, pack->dev->dev_addr, ETH_ALEN);
	memcpy(resp->nh.arph->tha, pack->mac.ethh->h_source, ETH_ALEN);
	resp->nh.arph->tpa = pack->nh.arph->spa;
	resp->nh.arph->spa = pack->nh.arph->tpa;

	arp_xmit(resp);
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
