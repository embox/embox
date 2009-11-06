/**
 * \file net_pack.c
 *
 * \date Mar 7, 2009
 * \author anton
 */
#include "conio.h"
#include "string.h"
#include "common.h"
#include "net/net_device.h"
#include "net/net_packet.h"
#include "lib/list.h"
#include "asm/spin_lock.h"
#include "kernel/module.h"


typedef struct _NET_BUFF_INFO {
	struct list_head list;
	unsigned char *buff;
}NET_PACK_INFO;

static LIST_HEAD(free_packet_list_head);
static LIST_HEAD(busy_packet_list_head);

static NET_PACK_INFO pack_pool[PACK_POOL_SIZE];
static unsigned char heap_buff[array_len(pack_pool)][ETHERNET_V2_FRAME_SIZE];

static int __init net_buff_init(){
	int i;

	for (i = 0; i < array_len(pack_pool); i ++){
		(&pack_pool[i])->buff = &heap_buff[i][0];
		list_add(&free_packet_list_head,&(&pack_pool[i])->list);
	}
	return 0;
}


unsigned char *net_buff_alloc() {

	unsigned long sp = spin_lock();
	if (list_empty (&free_packet_list_head)){
		spin_unlock(sp);
		return NULL;
	}
	list_move_tail((&free_packet_list_head)->next, &busy_packet_list_head);
	NET_PACK_INFO *pack = list_entry((&busy_packet_list_head)->prev, struct _NET_BUFF_INFO, list);

	spin_unlock(sp);
	return pack->buff;
}

void net_buff_free(unsigned char *buff) {
	NET_PACK_INFO *pack;
	struct list_head *q;
	unsigned long sp = spin_lock();
	list_for_each(q, &busy_packet_list_head){
		pack = list_entry(q, NET_PACK_INFO, list);
		if (pack->buff == buff) {
			list_move_tail(q, &free_packet_list_head);
			spin_unlock(sp);
			return;
		}
	}
	spin_unlock(sp);
}

