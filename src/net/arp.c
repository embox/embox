/**
 * \file arp.c
 *
 * \date Mar 11, 2009
 * \author anton
 */
#include "types.h"
#include "common.h"
#include "string.h"
#include "net.h"
#include "arp.h"
#include "net_device.h"
#include "eth.h"
#include "net_pack_manager.h"
#include "net.h"
#include "sock.h"

#define ARP_CACHE_SIZE 0x100

typedef struct _ARP_ENTITY
{
	unsigned char hw_addr[6];/**< hardware addr */
	unsigned char pw_addr[4];/**< protocol addr */
	void *if_handler;      /**< net_device */
	unsigned char is_busy;
}ARP_ENTITY;

#define ARP_REQUEST    0x1
#define ARP_RESPONSE   0x2

ARP_ENTITY arp_table[ARP_CACHE_SIZE];

#define ARP_TABLE_SIZE (sizeof(arp_table)/sizeof(arp_table[0]))

#define ARP_HARDWARE_TYPE_ETH (unsigned short)0x0001

static inline int find_entity(void *ifdev, unsigned char dst_addr[4])
{
	int i;
	for (i = 0; i < ARP_TABLE_SIZE; i++)
	{
		if(arp_table[i].is_busy && (0 == memcmp(arp_table[i].pw_addr, dst_addr, 4)) && (ifdev==arp_table[i].if_handler))
		{
			return i;
		}
	}
	return -1;
}

int arp_add_entity(void *ifdev, unsigned char ipaddr[4], unsigned char macaddr[6]) {
	LOGGER();
	int i;
	if (-1 != (i = find_entity(ifdev,ipaddr)))
		return i;

	for (i = 0; i < ARP_TABLE_SIZE; i++)
	{
		if(0 == arp_table[i].is_busy)
		{
			arp_table[i].is_busy = 1;
			arp_table[i].if_handler = ifdev;
			memcpy(arp_table[i].pw_addr, ipaddr, sizeof (arp_table[i].pw_addr));
			memcpy(arp_table[i].hw_addr, macaddr, sizeof(arp_table[i].hw_addr));
			return i;
		}
	}
}

static unsigned char brodcast_mac_addr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static inline net_packet* build_arp_pack(void *ifdev, unsigned char dst_addr[4])
{
	net_packet *pack;

	if (NULL == ifdev)
		return NULL;

	if (NULL == (pack = net_packet_alloc()))
		return NULL;

	pack->ifdev = ifdev;
	pack->netdev = eth_get_netdevice(ifdev);
	pack->mac.raw = pack->data;
//mac header
	memcpy (pack->mac.mach->dst_addr, brodcast_mac_addr, MAC_ADDR_LEN);
	memcpy (pack->mac.mach->src_addr, pack->netdev->hw_addr, MAC_ADDR_LEN);
	pack->mac.mach->type = ARP_PROTOCOL_TYPE;

	pack->nh.raw = pack->mac.raw + MAC_HEADER_SIZE;

//arp header
	pack->nh.arph->htype = ARP_HARDWARE_TYPE_ETH;
	pack->nh.arph->ptype = IP_PROTOCOL_TYPE;
	//TODO length hardware and logical type
	pack->nh.arph->hlen = pack->netdev->addr_len;
	pack->nh.arph->plen = 4;
	pack->nh.arph->oper = ARP_REQUEST;
	memcpy (pack->nh.arph->sha, pack->netdev->hw_addr, MAC_ADDR_LEN);
	memcpy (pack->nh.arph->spa, eth_get_ipaddr(ifdev), sizeof(pack->nh.arph->spa));
	memcpy (pack->nh.arph->tpa, dst_addr, sizeof(pack->nh.arph->tpa));

	pack->len = 0x3b;
	pack->protocol = ARP_PROTOCOL_TYPE;
	return pack;
}

net_packet *arp_resolve_addr (net_packet * pack, unsigned char dst_addr[4])
{
	int i;
	void *ifdev;
	if (NULL == pack || NULL == pack->ifdev)
		return NULL;

	ifdev = pack->ifdev;
	if (-1 != (i = find_entity(pack->ifdev, dst_addr)))
	{
		pack->mac.raw = pack->data;
		memcpy (pack->mac.mach->dst_addr, arp_table[i].hw_addr, sizeof(pack->mac.mach->dst_addr));
		return pack;
	}
	//send mac packet
	eth_send(build_arp_pack(pack->ifdev, dst_addr));
	sleep(500);
	if (-1 != (i = find_entity(pack->ifdev, dst_addr)))
	{
		pack->mac.raw = pack->data;
		memcpy (pack->mac.mach->dst_addr, arp_table[i].hw_addr, sizeof(pack->mac.mach->dst_addr));
		return pack;
	}
	return NULL;
}
static int received_resp(net_packet *pack)
{
	arphdr *arp = pack->nh.arph;
	if (0 != memcmp(eth_get_ipaddr(pack->ifdev), arp->tpa, 4))
		return 0;

	TRACE ("arp received resp from ip ");
	ipaddr_print(arp->spa, sizeof(arp->spa));
	TRACE (" mac ");
	macaddr_print(arp->sha, sizeof(arp->sha));

	//add to arp_table
	arp_add_entity(pack->ifdev, arp->spa, arp->sha);
	return 0;
}

int received_req(net_packet *pack)
{
	net_packet *resp;
	arphdr *arp = pack->nh.arph;

	TRACE ("arp received request from ip ");
	ipaddr_print(arp->spa, sizeof(arp->spa));
	TRACE (" mac ");
	macaddr_print(arp->sha, sizeof(arp->sha));

	//add to arp_table
	arp_add_entity(pack->ifdev, arp->spa, arp->sha);

	resp = net_packet_copy(pack);

	memcpy(resp->mac.mach->dst_addr, pack->mac.mach->src_addr, sizeof(resp->mac.mach->dst_addr));
	memcpy(resp->mac.mach->src_addr, pack->netdev->hw_addr, sizeof(resp->mac.mach->src_addr));
	resp->nh.arph->oper = ARP_RESPONSE;
	memcpy(resp->nh.arph->sha, pack->netdev->hw_addr, sizeof(resp->nh.arph->sha));
	memcpy(resp->nh.arph->tha, pack->mac.mach->src_addr, sizeof(resp->nh.arph->tha));
	memcpy(resp->nh.arph->tpa, pack->nh.arph->spa, sizeof(resp->nh.arph->tpa));
	memcpy(resp->nh.arph->spa, pack->nh.arph->tpa, sizeof(resp->nh.arph->spa));

	eth_send(resp);
	return 0;
}

int arp_received_packet(net_packet *pack)
{
	arphdr *arp = pack->nh.arph;

	if (0 != memcmp(eth_get_ipaddr(pack->ifdev), arp->tpa, 4))
		return 0;

	switch(arp->oper)
	{
	case ARP_RESPONSE:
		return received_resp(pack);

	case ARP_REQUEST:
		return received_req(pack);

	default:
		return 0;
	}

	return 0;

}

int arp_delete_entity(void *ifdev, unsigned char ipaddr[4], unsigned char macaddr[6]) {
	LOGGER();
        int i;
        for (i = 0; i < ARP_TABLE_SIZE; i++) {
                if(0 == memcmp(arp_table[i].pw_addr, ipaddr, 4)) {
                        arp_table[i].is_busy = 0;
                }
                if(0 == memcmp(arp_table[i].hw_addr, macaddr, 6)) {
            		arp_table[i].is_busy = 0;
            	}
            	if(ifdev == arp_table[i].if_handler) {
            		arp_table[i].is_busy = 0;
            	}
        }
}

int print_arp_cache() {
	LOGGER();
	int i;
	net_device *net_dev;
	for(i=0; i<ARP_CACHE_SIZE; i++) {
		if(arp_table[i].is_busy == 1) {
			net_dev = eth_get_netdevice(arp_table[i].if_handler);
			ipaddr_print(arp_table[i].pw_addr, 4);
			printf("\t\t%d\t", eth_get_netdevice(arp_table[i].if_handler)->type);
			macaddr_print(arp_table[i].hw_addr, 6);
			printf("\t%d", net_dev->flags);
			printf("\t%s\n", net_dev->name);
		}
	}
	return 0;
}
