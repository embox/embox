/*
 * arp.c
 *
 *  Created on: Mar 11, 2009
 *      Author: anton
 */
#include "types.h"
#include "string.h"
#include "net.h"
#include "arp.h"
#include "net_device.h"
#include "eth.h"
#include "net_pack_manager.h"
#include "net.h"


typedef struct _ARP_ENTITY
{
	unsigned char hw_addr[6];
	unsigned char pw_addr[4];//protocol addr
	void *if_handler;
}ARP_ENTITY;
#define ARP_REQUEST 0x1
ARP_ENTITY arp_table[0x100];
#define ARP_TABLE_SIZE (sizeof(arp_table)/sizeof(arp_table[0]))
/*
typedef struct _ARP_PACKET
{
	unsigned char hw_dst_addr[6];// = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	unsigned char hw_src[6];
	unsigned char hw_type[2];// = {0x08, 0x06};
	unsigned short htype;// = 0x0001;//ethernet
	unsigned short ptype;// = 0x0800;//ip
	unsigned short oper;//1 req 2 resp
	unsigned char sha[6];//Sender hardware address
	unsigned char spa[4];//Sender protocol address
	unsigned char tha[6];//Target hardware address
	unsigned char tpa[4];//Target protocol address
}ARP_PACKET;
*/

ARP_RESULT *arp_resolve_addr(void *ifdev, unsigned char dst_addr[4], ARP_RESULT *res)
{
	int i;
	for (i = 0; i < ARP_TABLE_SIZE; i++)
	{
		if(0 == memcmp(arp_table[i].pw_addr, dst_addr, 4))
		{
			res->hw_dstaddr = arp_table[i].hw_addr;
			res->if_handler = arp_table[i].if_handler;
			return res;
		}
	}
	//TODO send mac packet
	return NULL;
}

int arp_received_packet(net_packet *pack)
{
	net_packet *resp;
	arphdr *arp = pack->nh.arph;


	if(ARP_REQUEST != arp->oper)
		return 0;

	if (0 != memcmp(eth_get_ipaddr(pack->ifdev), arp->tpa, 4))
		return 0;

	//TODO add to arp table
	resp = net_packet_copy(pack);
//TODO fix absolute size of mac addr
	memcpy(resp->mac.mach->dst_addr, pack->mac.mach->src_addr, 6);
	memcpy(resp->mac.mach->src_addr, pack->netdev->hw_addr, 6);
	resp->nh.arph->oper = 2;
	memcpy(resp->nh.arph->tha, pack->netdev->hw_addr, 6);

	eth_send(pack);
}
