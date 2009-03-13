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
	//void *tx_pack_manager;
	//void *rx_pack_manager;
}IF_DEVICE;

static IF_DEVICE ifs[INTERFACES_QUANTITY];


static int rebuild_headers(net_packet * pack)
{
/*	ARP_RESULT arp_res;
	arp_res.if_handler = handler;
	if(!arp_resolve_addr(handler, dest_addr, &arp_res))
		return -1;

	dev->net_dev->rebuild_header(pack);
	pack = net_packet_alloc();*/
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
			/*if (NULL == (ifs[i].rx_pack_manager = net_pack_manager_alloc()))
			{
				TRACE("ERROR: ifs %d can't alloc rx_pack_manager\n", i);
			}
			if ((void*)NULL == (ifs[i].tx_pack_manager = net_pack_manager_alloc()))
			{
				TRACE("ERROR: ifs %d can't alloc tx_pack_manager\n", i);
			}*/
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
/*
static inline net_packet * lock_free_rx_pack(IF_DEVICE *dev, const unsigned char *data, int len)
{
	return net_pack_manager_lock_pack(dev->rx_pack_manager, data, len);
}

static inline net_packet * lock_free_tx_pack(IF_DEVICE *dev, const unsigned char *data, int len)
{
	return net_pack_manager_lock_pack(dev->tx_pack_manager, data, len);
}
*/

unsigned char *eth_get_ipaddr(void *handler)
{
	IF_DEVICE *dev = (IF_DEVICE *)handler;
	if (NULL == dev)
		return NULL;
	return dev->ipv4_addr;
}

//int eth_send (void *handler, unsigned char dest_addr[4], unsigned int type, const unsigned char *data, int len)
int eth_send (net_packet *pack)
{
	IF_DEVICE *dev = (IF_DEVICE *)pack->ifdev;

	if ((NULL == pack->ifdev) || (NULL == pack))
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
		arp_received_packet(pack);
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


