/**
 * @date 13.12.2011
 * @author Alexander Kalmuk
 */

#include <net/defpack_resolve.h>
#include <types.h>
#include <util/list.h>
#include <net/skbuff.h>
#include <net/ip.h>
#include <mem/objalloc.h>
#include <net/inetdevice.h>
#include <kernel/timer.h>
#include <net/arp.h>
#include <string.h>

struct deff_packet {
	struct list_head link;
	sk_buff_t *skb;
	sys_timer_t *timer;
};

OBJALLOC_DEF(__deff_packet_buff, struct deff_packet, MAX_PACKET_CNT);

static LIST_HEAD(deff_packet_list);

void def_process_list(sk_buff_t *arp_pack) {
	struct deff_packet *pack, *n;

	list_for_each_entry_safe(pack, n, &deff_packet_list, link) {
		if(arp_pack->nh.arph->ar_sip == pack->skb->nh.iph->daddr) {
			list_del(&pack->link);
			timer_close(pack->timer);
			memcpy(pack->skb->mac.ethh->h_dest, arp_pack->mac.ethh->h_source, ETH_ALEN);
			pack->skb->sk->answer = dev_queue_xmit(pack->skb);
			pack->skb->sk->is_ready = 1;
			pool_free(&__deff_packet_buff, pack);
		}
	}
}

static void free_packet(struct sys_timer *timer, void *data) {
	struct deff_packet *deff_pack;

	/* it must send message, that packet was dropped */
	deff_pack = (struct deff_packet*) data;
	list_del(&deff_pack->link);
	kfree_skb(deff_pack->skb);
	timer_close(timer);
	deff_pack->skb->sk->is_ready = 1;
	deff_pack->skb->sk->answer = -1;
	pool_free(&__deff_packet_buff, deff_pack);
}

void def_add_packet(sk_buff_t *pack) {
	sys_timer_t *timer;
	struct deff_packet *deff_pack;

	deff_pack = (struct deff_packet*) pool_alloc(&__deff_packet_buff);
	timer_set(&timer, DEF_PACKET_TTL, free_packet, deff_pack);

	INIT_LIST_HEAD(&deff_pack->link);
	deff_pack->skb = pack;
	deff_pack->timer = timer;
	list_add_tail(&deff_packet_list, &deff_pack->link);
	deff_pack->skb->sk->is_ready = 0;
}
