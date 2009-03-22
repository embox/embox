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
#include "net.h"
#include "eth.h"

#define INTERFACES_QUANTITY 8
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

/*
int rebuild_ip_header(net_packet * pack)
{
	if (NULL == pack->nh.raw)
			return -1;
	memcpy(&pack->data[sizeof(machdr) - sizeof(pack->mac.mach->raw)], pack->nh.iph, sizeof(iphdr) - sizeof (pack->nh.iph->raw));

}
*/
static int rebuild_headers(net_packet * pack)
{
	if (SOCK_RAW != pack->sk->sk_type)
	{
		if (NULL == arp_resolve_addr(pack, pack->nh.iph->dst_addr));
			return -1;
		memcpy (pack->mac.mach->src_addr, pack->netdev->hw_addr, sizeof(pack->mac.mach->src_addr));
		return 0;
	}
	return 0;
}

int eth_init()
{
	char iname[6];
	int cnt = 0, i;
	TRACE("Initializing ifdevs:\n");
	for (i = 0; i < INTERFACES_QUANTITY; i ++)
	{
		sprintf(iname, "eth%d\n", i);
		if (NULL != (ifs[i].net_dev = find_net_device(iname)))
		{
			TRACE("Found dev %s\n", iname);
			cnt ++;
		}
	}
	return cnt;
}

void *eth_get_if(const char *if_name)
{
	int i;
	for (i = 0; i < INTERFACES_QUANTITY; i ++)
	{
		if(0 == strcmp(if_name, ifs[i].net_dev->name))
		{
			return &ifs[i];
		}
	}
	return NULL;
}


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
		dev->net_dev->rebuild_header(pack);
	}
	dev->net_dev->hard_start_xmit(pack, pack->netdev);
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
		TRACE("%d:", addr[i]);
	}
	TRACE("%d", addr[i]);
}

int netif_rx(net_packet *pack)
{
	int i;

	IF_DEVICE *dev;
	if((NULL == pack->netdev) || (NULL == pack))
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
	}

	if (IP_PROTOCOL_TYPE == pack->protocol)
	{
		pack->nh.raw = pack->data + sizeof(machdr);
		pack->h.raw = pack->nh.raw + sizeof(iphdr);
		if (ICMP_PROTO_TYPE == pack->nh.iph->proto)
		{
			icmp_received_packet(pack);
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


