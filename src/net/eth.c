/*
 * eth.c
 *
 *  Created on: Mar 4, 2009
 *      Author: anton
 */
#include "types.h"
#include "common.h"
#include "string.h"
#include "net_device.h"
#include "net_pack_manager.h"
#include "arp.h"
#include "udp.h"
#include "net.h"
#include "eth.h"

#define INTERFACES_QUANTITY 4
typedef struct _CALLBACK_INFO
{
	int is_busy;
	unsigned short type;
	ETH_LISTEN_CALLBACK func;
}CALLBACK_INFO;

typedef struct _IF_DEVICE
{
	unsigned char ipv4_addr[4];
	net_device *net_dev;
	CALLBACK_INFO cb_info[8];
}IF_DEVICE;

static IF_DEVICE ifs[INTERFACES_QUANTITY];


static int rebuild_header(net_packet * pack)
{
	if (NULL == pack)
		return -1;

	if (NULL == pack->sk || SOCK_RAW != pack->sk->sk_type)
	{
		if (NULL == arp_resolve_addr(pack, pack->nh.iph->dst_addr))
		{
			TRACE ("Destanation host Unreachable\n");
			return -1;
		}
		memcpy (pack->mac.mach->src_addr, pack->netdev->hw_addr, sizeof(pack->mac.mach->src_addr));
		pack->mac.mach->type=pack->protocol;
		return 0;
	}
	return 0;
}

static unsigned char def_ip [4] = {192, 168, 0, 80};
static unsigned char def_mac [6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x00};

int eth_init()
{
	char iname[6];
	int cnt = 0, i;
	TRACE("Initializing ifdevs:\n");
	for (i = 0; i < INTERFACES_QUANTITY; i ++)
	{
		sprintf(iname, "eth%d", i);
		if (NULL != (ifs[i].net_dev = find_net_device(iname)))
		{
			def_ip[3] = 80 + i;
			eth_set_ipaddr(&ifs[i], def_ip);
			def_mac[5] = 1 + i;
			eth_set_macaddr(&ifs[i], def_mac);
			ifs[i].net_dev->rebuild_header = rebuild_header;
			TRACE("Found dev %s\n", iname);
			cnt ++;
		}
	}
	return cnt;
}

void *eth_get_ifdev_by_name(const char *if_name)
{
	int i;
	for (i = 0; i < INTERFACES_QUANTITY; i ++)
	{
		TRACE ("ifname %s, net_dev 0x%X\n", if_name, ifs[i].net_dev);
		if(0 == strncmp(if_name, ifs[i].net_dev->name, sizeof(ifs[i].net_dev->name)))
		{
			return &ifs[i];
		}
	}
	return NULL;
}

unsigned char *ipaddr_scan(unsigned char *addr, unsigned char res[4])
{
	unsigned int tmp;
	int i, j;
	for (i = 0; i < 3; i ++)
	{
		for (j = 0; j < 5; j ++)
		{
			if (j > 4)
				return NULL;

			if ('.' == addr[j])
			{
				addr [j] = 0;
				if (1 != sscanf (addr, "%d", &tmp))
					return NULL;
				if (tmp > 255)
					return NULL;
				res[i]=(unsigned char )0xFF & tmp;
				addr += (j + 1);
				break;
			}
		}
	}
	if (1 != sscanf (addr, "%d", &tmp))
		return NULL;
	if (tmp > 255)
		return NULL;
	res[3]=(unsigned char )0xFF & tmp;

	return 	res;
}

void ipaddr_print(unsigned char *addr, int len)
{
	int i;
	for (i = 0; i < (len - 1); i++)
	{
		TRACE("%d.", addr[i]);
	}
	TRACE("%d", addr[i]);
}

void macaddr_print(unsigned char *addr, int len)
{
	int i;
	for (i = 0; i < (len - 1); i++)
	{
		TRACE("%2X:", addr[i]);
	}
	TRACE("%X", addr[i]);
}
int eth_set_interface (char *name, char *ipaddr, char *macaddr) {
	int i;
	for (i = 0; i < INTERFACES_QUANTITY; i++ ) {
		if (0 == strcmp(name, ifs[i].net_dev->name)) {
			memcpy(ifs[i].ipv4_addr, ipaddr, 4 * sizeof(char));
			memcpy(ifs[i].net_dev->hw_addr, macaddr ,6 * sizeof(char));
			return i;
		}
	}
	return -1;
}

int eth_set_ipaddr (void *ifdev, unsigned char ipaddr[4]) {
	IF_DEVICE *dev = (IF_DEVICE *)ifdev;
	if (NULL == ifdev)
		return -1;
	memcpy(dev->ipv4_addr, ipaddr, sizeof(dev->ipv4_addr));
	return 0;
}

int eth_set_macaddr (void *ifdev, unsigned char macaddr[6]) {
	IF_DEVICE *dev = (IF_DEVICE *)ifdev;
	if (NULL == ifdev)
		return -1;
	memcpy(dev->net_dev->hw_addr, macaddr ,dev->net_dev->addr_len);
	return 0;
}
/*
int eth_set_interface (char *name, char *ipaddr, char *macaddr) {
	int i;
	for (i = 0; i < INTERFACES_QUANTITY; i++ ) {
		if (0 == strcmp(name, ifs[i].net_dev->name)) {
			memcpy(ifs[i].ipv4_addr, ipaddr, 4 * sizeof(char));
			memcpy(ifs[i].net_dev->hw_addr, macaddr ,6 * sizeof(char));
			return i;
		}
	}
	return -1;
}
*/
unsigned char *eth_get_ipaddr(void *handler)
{
	IF_DEVICE *dev = (IF_DEVICE *)handler;
	if (NULL == dev)
		return NULL;
	return dev->ipv4_addr;
}

net_device *eth_get_netdevice(void *handler)
{
	IF_DEVICE *dev = (IF_DEVICE *)handler;
	if (NULL == dev)
		return NULL;
	return dev->net_dev;
}

int eth_send (net_packet *pack)
{
	IF_DEVICE *dev = (IF_DEVICE *)pack->ifdev;

	if ((NULL == pack) || (NULL == pack->ifdev))
		return -1;

	if (ARP_PROTOCOL_TYPE != pack->protocol)
	{
		if (-1 == dev->net_dev->rebuild_header(pack))
		{
			net_packet_free(pack);
			return -1;
		}
	}
	dev->net_dev->hard_start_xmit(pack, pack->netdev);
	net_packet_free(pack);
	return 0;
}

static int alloc_callback (IF_DEVICE *dev, unsigned int type, ETH_LISTEN_CALLBACK callback)
{
	int i;
	for (i = 0; i < sizeof(dev->cb_info) / sizeof(dev->cb_info[0]); i ++)
	{
		if(0 == dev->cb_info[i].is_busy)
		{
			dev->cb_info[i].is_busy = 1;
			dev->cb_info[i].type = type;
			dev->cb_info[i].func = callback;
			return i;
		}
	}
	return -1;
}


int netif_rx(net_packet *pack)
{
	int i;

	IF_DEVICE *dev;
	if((NULL == pack) || (NULL == pack->netdev))
		return -1;

	pack->nh.raw = (void *)pack->data + sizeof(machdr);

	for(i = 0; i < sizeof(ifs)/ sizeof (ifs[0]); i ++)
	{
		if (ifs[i].net_dev == pack->netdev)
		{
			pack->ifdev = &ifs[i];
			break;
		}
	}

	if (ARP_PROTOCOL_TYPE == pack->protocol)
	{
		pack->nh.raw = pack->data + sizeof(machdr);
		arp_received_packet(pack);
		net_packet_free(pack);
		return;
	}

	if (IP_PROTOCOL_TYPE == pack->protocol)
	{
		pack->nh.raw = pack->data + sizeof(machdr);
		pack->h.raw = pack->nh.raw + sizeof(iphdr);
		if (ICMP_PROTO_TYPE == pack->nh.iph->proto)
		{
			icmp_received_packet(pack);
		}
		if (UDP_PROTO_TYPE == pack->nh.iph->proto)
		{
			udp_received_packet(pack);
		}
	}
	dev = (IF_DEVICE *)pack->ifdev;
	for (i = 0; i < sizeof(dev->cb_info) / sizeof(dev->cb_info[0]); i ++)
	{
		if(1 == dev->cb_info[i].is_busy)
		{
			if ((NET_TYPE_ALL_PROTOCOL == dev->cb_info[i].type) || (dev->cb_info[i].type == pack->protocol))
			{
				//may be copy pack for different protocols
				dev->cb_info[i].func(pack);
			}
		}
	}
	net_packet_free(pack);
}

int eth_listen (void *handler, unsigned short type, ETH_LISTEN_CALLBACK callback)
{
	IF_DEVICE *dev = (IF_DEVICE *)handler;
	if (NULL == dev)
		return -1;

	return alloc_callback(dev, type, callback);
}


