/**
 * \file arp.c
 *
 * \date Mar 11, 2009
 * \author anton
 */
#include "conio.h"
#include "common.h"
#include "string.h"
#include "net/skbuff.h"
#include "net/net_device.h"
#include "net/net.h"
#include "net/if_device.h"
#include "net/eth.h"
#include "net/net_pack_manager.h"
#include "net/if_arp.h"
#include "net/arp.h"



//TODO this is wrong place for this variable
static unsigned char broadcast_mac_addr[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static unsigned char broadcast_ip_addr[IPV4_ADDR_LENGTH] = {0xFF, 0xFF, 0xFF, 0xFF};

ARP_ENTITY arp_table[ARP_CACHE_SIZE];

#define ARP_TABLE_SIZE array_len(arp_table)

static inline int find_entity(void *ifdev, unsigned char dst_addr[IPV4_ADDR_LENGTH]) {
	int i;
	for (i = 0; i < ARP_TABLE_SIZE; i++) {
		if(arp_table[i].is_busy &&
		  (0 == memcmp(arp_table[i].pw_addr, dst_addr, sizeof(arp_table[i].pw_addr))) &&
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
int arp_add_entity(void *ifdev, unsigned char ipaddr[IPV4_ADDR_LENGTH], unsigned char macaddr[ETH_ALEN]) {
	int i;
	if (-1 != (i = find_entity(ifdev,ipaddr))) {
		return i;
	}
	for (i = 0; i < ARP_TABLE_SIZE; i++) {
		if(0 == arp_table[i].is_busy) {
			arp_table[i].is_busy = 1;
			arp_table[i].if_handler = ifdev;
			memcpy(arp_table[i].pw_addr, ipaddr, sizeof (arp_table[i].pw_addr));
			memcpy(arp_table[i].hw_addr, macaddr, sizeof(arp_table[i].hw_addr));
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
int arp_delete_entity(void *ifdev, unsigned char ipaddr[IPV4_ADDR_LENGTH], unsigned char macaddr[ETH_ALEN]) {
    int i;
    for (i = 0; i < ARP_TABLE_SIZE; i++) {
       if(0 == memcmp(arp_table[i].pw_addr, ipaddr, array_len(arp_table[i].pw_addr)) ||
           0 == memcmp(arp_table[i].hw_addr, macaddr, ETH_ALEN) ||
           ifdev == arp_table[i].if_handler) {
               arp_table[i].is_busy = 0;
       }
    }
    return -1;
}

static inline sk_buff_t* build_arp_pack(void *ifdev, unsigned char dst_addr[IPV4_ADDR_LENGTH]) {
	sk_buff_t *pack;

	if (NULL == ifdev ||
	    NULL == (pack = alloc_skb((int)pack->len, 0))) {
		return NULL;
	}

#if 0
	pack->ifdev   = ifdev;
#endif
	pack->netdev  = inet_dev_get_netdevice(ifdev);
	pack->mac.raw = pack->data;
	/* mac header */
	memcpy (pack->mac.ethh->h_dest, broadcast_mac_addr, ETH_ALEN);
	memcpy (pack->mac.ethh->h_source, pack->netdev->hw_addr, ETH_ALEN);
	pack->mac.ethh->h_proto = ETH_P_ARP;

	pack->nh.raw = pack->mac.raw + ETH_HEADER_SIZE;

	/* arp header */
	pack->nh.arph->htype = ARPHRD_ETHER;
	pack->nh.arph->ptype = ETH_P_IP;
	//TODO length hardware and logical type
	pack->nh.arph->hlen = pack->netdev->addr_len;
	pack->nh.arph->plen = IPV4_ADDR_LENGTH;
	pack->nh.arph->oper = ARPOP_REQUEST;
	memcpy (pack->nh.arph->sha, pack->netdev->hw_addr, ETH_ALEN);
	memcpy (pack->nh.arph->spa, inet_dev_get_ipaddr(ifdev), sizeof(pack->nh.arph->spa));
	memcpy (pack->nh.arph->tpa, dst_addr, sizeof(pack->nh.arph->tpa));

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
sk_buff_t *arp_resolve_addr (sk_buff_t * pack, unsigned char dst_addr[IPV4_ADDR_LENGTH]) {
	int i;
#if 0
	void *ifdev = pack->ifdev;

	if (NULL == pack || NULL == ifdev) {
		return NULL;
	}
#endif
	if (memcmp(dst_addr, broadcast_ip_addr, sizeof(dst_addr))){
		pack->mac.raw = pack->data;
		memcpy (pack->mac.ethh->h_dest, arp_table[i].hw_addr, sizeof(pack->mac.ethh->h_dest));
		return pack;
	}
	//TODO modify arp table
#if 0
	if (-1 != (i = find_entity(ifdev, dst_addr))) {
		pack->mac.raw = pack->data;
		memcpy (pack->mac.ethh->h_dest, broadcast_mac_addr, sizeof(pack->mac.ethh->h_dest));
		return pack;
	}

	/*send mac packet*/
	dev_queue_xmit(build_arp_pack(ifdev, dst_addr));

	//TODO delete this after processes will be added to monitor
	sleep(500);
	if (-1 != (i = find_entity(ifdev, dst_addr))) {
		pack->mac.raw = pack->data;
		memcpy (pack->mac.ethh->h_dest, arp_table[i].hw_addr, sizeof(pack->mac.ethh->h_dest));
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
	if (0 != memcmp(inet_dev_get_ipaddr(pack->ifdev), arp->tpa, array_len(arp->tpa))) {
		return -1;
	}
#endif
	//TODO delete this out log output from arp protocol in usual mode
	char ip[15], mac[18];
	ipaddr_print(ip, arp->spa);
	macaddr_print(mac, arp->sha);
	LOG_DEBUG ("arp received resp from ip %s mac %s", ip, mac);

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
	char ip[15], mac[18];
	ipaddr_print(ip, arp->spa);
	macaddr_print(mac, arp->sha);
	LOG_DEBUG ("arp received request from ip %s mac %s", ip, mac);

	/*add record into arp_table*/
	//TODO modify arp table
#if 0
	arp_add_entity(pack->ifdev, arp->spa, arp->sha);
#endif
	resp = skb_copy(pack, 0);

	memcpy(resp->mac.ethh->h_dest, pack->mac.ethh->h_source, sizeof(resp->mac.ethh->h_dest));
	memcpy(resp->mac.ethh->h_source, pack->netdev->hw_addr, sizeof(resp->mac.ethh->h_source));
	resp->nh.arph->oper = ARPOP_REPLY;
	memcpy(resp->nh.arph->sha, pack->netdev->hw_addr, sizeof(resp->nh.arph->sha));
	memcpy(resp->nh.arph->tha, pack->mac.ethh->h_source, sizeof(resp->nh.arph->tha));
	memcpy(resp->nh.arph->tpa, pack->nh.arph->spa, sizeof(resp->nh.arph->tpa));
	memcpy(resp->nh.arph->spa, pack->nh.arph->tpa, sizeof(resp->nh.arph->spa));

	dev_queue_xmit(resp);
	return 0;
}

/**
 * Handle arp packet. This function called protocal stack when arp packet has been received
 * @param pack net_packet
 */
int arp_received_packet(sk_buff_t *pack) {
	LOG_WARN("arp packet received\n");
	arphdr_t *arp = pack->nh.arph;
	//TODO need add function for getting ip addr
#if 0
	if (0 != memcmp(inet_dev_get_ipaddr(pack->ifdev), arp->tpa, array_len(arp->tpa))) {
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
