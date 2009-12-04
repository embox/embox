/**
 * @file arp.c
 *
 * @brief This module implements the Address Resolution Protocol ARP (RFC 826),
 * which is used to convert IP addresses into a low-level hardware address.
 * @date 11.03.2009
 * @author Anton Bondarev
 */
#include "string.h"
#include "common.h"
#include "lib/inet/netinet/in.h"
#include "kernel/module.h"
#include "net/skbuff.h"
#include <net/netdevice.h>
#include "net/net.h"
#include "net/inetdevice.h"
#include "net/etherdevice.h"
#include "net/net_pack_manager.h"
#include <net/arp.h>

//TODO this is wrong place for this variable
static unsigned char broadcast_mac_addr[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

ARP_ENTITY arp_table[ARP_CACHE_SIZE];

#define ARP_TABLE_SIZE array_len(arp_table)

static struct packet_type arp_packet_type = {
        .type = ETH_P_ARP,
        .func = arp_rcv,
};

void __init arp_init() {
        dev_add_pack(&arp_packet_type);
}

static inline int find_entity(void *ifdev, in_addr_t dst_addr) {
	int i;
	for (i = 0; i < ARP_TABLE_SIZE; i++) {
		if(arp_table[i].is_busy &&
		  (arp_table[i].pw_addr == dst_addr) &&
		  (ifdev == arp_table[i].if_handler)) {
			return i;
		}
	}
	return -1;
}

/**
 * this function add entry in arp table if can
 * @param ifdev (handler of ifdev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return number of entry in table if success else -1
 */
int arp_add_entity(void *ifdev, in_addr_t ipaddr, unsigned char macaddr[ETH_ALEN]) {
	int i;
	if (-1 != (i = find_entity(ifdev,ipaddr))) {
		return i;
	}
	for (i = 0; i < ARP_TABLE_SIZE; i++) {
		if(0 == arp_table[i].is_busy) {
			arp_table[i].is_busy = 1;
			arp_table[i].if_handler = ifdev;
			arp_table[i].pw_addr = ipaddr;
			memcpy(arp_table[i].hw_addr, macaddr, ETH_ALEN);
			return i;
		}
	}
	return -1;
}

/**
 * this function delete entry from arp table if can
 * @param ifdev (handler of ifdev struct) which identificate network interface where address can resolve
 * @param ip addr
 * @param hardware addr
 * @return number of entry in table if success else -1
 */
int arp_delete_entity(void *ifdev, in_addr_t ipaddr, unsigned char macaddr[ETH_ALEN]) {
    int i;
    for (i = 0; i < ARP_TABLE_SIZE; i++) {
       if( arp_table[i].pw_addr == ipaddr ||
           0 == memcmp(arp_table[i].hw_addr, macaddr, ETH_ALEN) ||
           ifdev == arp_table[i].if_handler) {
               arp_table[i].is_busy = 0;
       }
    }
    return -1;
}

static inline sk_buff_t* build_arp_pack(void *ifdev, in_addr_t dst_addr) {
	sk_buff_t *pack;

	if (NULL == ifdev ||
	    NULL == (pack = alloc_skb((int)pack->len, 0))) {
		return NULL;
	}

#if 0
	pack->ifdev   = ifdev;
#endif
	pack->dev  = inet_dev_get_netdevice(ifdev);
	pack->mac.raw = pack->data;
	/* mac header */
	memcpy (pack->mac.ethh->h_dest, broadcast_mac_addr, ETH_ALEN);
	memcpy (pack->mac.ethh->h_source, pack->dev->hw_addr, ETH_ALEN);
	pack->mac.ethh->h_proto = ETH_P_ARP;

	pack->nh.raw = pack->mac.raw + ETH_HEADER_SIZE;

	/* arp header */
	pack->nh.arph->htype = ARPHRD_ETHER;
	pack->nh.arph->ptype = ETH_P_IP;
	//TODO length hardware and logical type
	pack->nh.arph->hlen = pack->dev->addr_len;
	pack->nh.arph->plen = IPV4_ADDR_LENGTH;
	pack->nh.arph->oper = ARPOP_REQUEST;
	memcpy (pack->nh.arph->sha, pack->dev->hw_addr, ETH_ALEN);
	pack->nh.arph->spa = inet_dev_get_ipaddr(ifdev);
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
sk_buff_t *arp_resolve_addr (sk_buff_t *pack, in_addr_t dst_addr) {
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
	//TODO modify arp table
#if 0
	if (-1 != (i = find_entity(ifdev, dst_addr))) {
		pack->mac.raw = pack->data;
		memcpy (pack->mac.ethh->h_dest, broadcast_mac_addr, ETH_ALEN);
		return pack;
	}

	/*send mac packet*/
	dev_queue_xmit(build_arp_pack(ifdev, dst_addr));

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
	//TODO delete this out log output from arp protocol in usual mode
	char mac[18];
	macaddr_print(mac, arp->sha);
	struct in_addr spa;
	spa.s_addr = arp->spa;
	LOG_DEBUG ("arp received resp from ip %s mac %s", inet_ntoa(spa), mac);

	//add to arp_table
	//TODO modify arp table
#if 0
	arp_add_entity(pack->ifdev, arp->spa, arp->sha);
#endif
	return 0;
}

/**
 * receive ARP request, send ARP response
 */
static int received_req(sk_buff_t *pack) {
	sk_buff_t *resp;
	arphdr_t *arp = pack->nh.arph;
	char mac[18];
	macaddr_print(mac, arp->sha);
	struct in_addr spa;
	spa.s_addr = arp->spa;
	LOG_DEBUG ("arp received request from ip %s mac %s", inet_ntoa(spa), mac);

	/*add record into arp_table*/
	//TODO modify arp table
#if 0
	arp_add_entity(pack->ifdev, arp->spa, arp->sha);
#endif
	resp = skb_copy(pack, 0);

	memcpy(resp->mac.ethh->h_dest, pack->mac.ethh->h_source, ETH_ALEN);
	memcpy(resp->mac.ethh->h_source, pack->dev->hw_addr, ETH_ALEN);
	resp->nh.arph->oper = ARPOP_REPLY;
	memcpy(resp->nh.arph->sha, pack->dev->hw_addr, ETH_ALEN);
	memcpy(resp->nh.arph->tha, pack->mac.ethh->h_source, ETH_ALEN);
	resp->nh.arph->tpa = pack->nh.arph->spa;
	resp->nh.arph->spa = pack->nh.arph->tpa;

	dev_queue_xmit(resp);
	return 0;
}

int arp_rcv(sk_buff_t *pack, net_device_t *dev,
                   packet_type_t *pt, net_device_t *orig_dev) {
	LOG_WARN("arp packet received\n");
	arphdr_t *arp = pack->nh.arph;
	//TODO need add function for getting ip addr
#if 0
	if (inet_dev_get_ipaddr(pack->ifdev) != arp->tpa) {
		return 0;
	}
#endif
	switch(arp->oper) {
	case ARPOP_REPLY:
		return received_resp(pack);

	case ARPOP_REQUEST:
		return received_req(pack);

	default:
		return 0;
	}

	return 0;
}
