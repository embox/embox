/**
 * \file net_pack.c
 *
 * \date Mar 7, 2009
 * \author anton
 */
#include "types.h"
#include "common.h"
#include "net_device.h"
#include "net_pack_manager.h"

#define PACK_POOL_SIZE	0x100

typedef struct _NET_PACK_INFO
{
	net_packet pack;
	int is_busy;
}NET_PACK_INFO;

static NET_PACK_INFO pack_pool[PACK_POOL_SIZE];

net_packet *net_packet_alloc()
{
	int i;
	for(i = 0; i < PACK_POOL_SIZE; i ++)
	{
		if (0 == pack_pool[i].is_busy)
		{
			pack_pool[i].is_busy = 1;
			//clear all references
			memset(&pack_pool[i].pack, 0, sizeof(pack_pool[i].pack) - sizeof(pack_pool[i].pack.data));
			return &pack_pool[i].pack;
		}
	}
	return NULL;
}

void net_packet_free(net_packet *pack)
{
	int i;
	for(i = 0; i < PACK_POOL_SIZE; i ++)
	{
		if (pack == &pack_pool[i].pack)
		{
			pack_pool[i].is_busy = 0;
		}
	}
}
net_packet *net_packet_copy(net_packet *pack)
{
	net_packet *new_pack;
	if (NULL == pack)
		return NULL;
	if (NULL == (new_pack = net_packet_alloc()))
		return NULL;
//TODO too much copy during copy net_packets
	memcpy (new_pack, pack, sizeof(net_packet));

	//fix references during copy net_pack
	if (NULL != pack->h.raw)
		new_pack->h.raw = new_pack->data + (pack->h.raw - pack->data);

	if (NULL != pack->nh.raw)
			new_pack->nh.raw = new_pack->data + (pack->nh.raw - pack->data);

	if (NULL != pack->mac.raw)
			new_pack->mac.raw = new_pack->data + (pack->mac.raw - pack->data);

	return new_pack;
}


