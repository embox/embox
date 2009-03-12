/*
 * net_pack.c
 *
 *  Created on: Mar 7, 2009
 *      Author: anton
 */
#include "types.h"
#include "common.h"
#include "net_device.h"
#include "net_pack_manager.h"

typedef struct _NET_PACK_INFO
{
	net_packet pack;
	int is_busy;
}NET_PACK_INFO;

typedef struct _NET_PACK_MANAGER
{
	NET_PACK_INFO packs[4];
	int is_busy;
}NET_PACK_MANAGER;
static NET_PACK_MANAGER managers[16+16];//8 interfaces x 2 + raw packet for driver
int net_packet_manager_init()
{
	return 0;
}

void *net_pack_manager_alloc()
{
	int i;
	for(i = 0; i < sizeof(managers) / sizeof(managers[0]); i ++)
	{
		if (0 == managers[i].is_busy)
		{
			managers[i].is_busy = 1;
			return &managers[i];
		}
	}
	return NULL;
}

void *net_pack_manager_lock_pack(void *manager,  const unsigned char * data, unsigned short len)
{
	int i;
	NET_PACK_MANAGER *man = (NET_PACK_MANAGER *)manager;
	if (NULL == manager)
		return NULL;
	for (i = 0; i < sizeof(man->packs) / sizeof(man->packs[0]); i ++)
	{
		if(0 == man->packs[i].is_busy)
		{
			man->packs[i].is_busy = 1;
			memcpy(man->packs[i].pack.data, data, len);
			return &man->packs[i];
		}
	}
	return NULL;
}

void net_pack_manager_unlock_pack(void *manager, void *net_pack_info)
{
	int i;
	NET_PACK_INFO *pack = (NET_PACK_INFO *)net_pack_info;
	if (NULL == pack)
		return ;
	pack->is_busy = 1;
}
