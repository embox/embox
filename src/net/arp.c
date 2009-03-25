/*
 * arp.c
 *
 *  Created on: Mar 11, 2009
 *      Author: anton
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


typedef struct _ARP_ENTITY
{
	unsigned char hw_addr[6];
	unsigned char pw_addr[4];//protocol addr
	void *if_handler;
	unsigned char is_busy;
}ARP_ENTITY;

#define ARP_REQUEST    0x1
#define ARP_RESPONSE   0x2

ARP_ENTITY arp_table[0x100];

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

static inline int add_entity(void *ifdev, unsigned char ipaddr[4], unsigned char macaddr[6])
{
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
	add_entity(pack->ifdev, arp->spa, arp->sha);
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
	add_entity(pack->ifdev, arp->spa, arp->sha);

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
//TODO delete_interface_from arp table
//TODO delete_targer from arp table
