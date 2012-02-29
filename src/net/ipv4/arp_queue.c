/**
 * @file
 * @brief Handle queue of packets waiting for ARP resolving.
 *
 * @date 13.12.2011
 * @author Alexander Kalmuk
 */

#include <net/arp_queue.h>
#include <types.h>
#include <util/list.h>
#include <mem/objalloc.h>
#include <errno.h>
#include <net/skbuff.h>
#include <net/ip.h>
#include <net/inetdevice.h>
#include <net/arp.h>
#include <kernel/timer.h>

#define TTL 1000
#define MAX_QUEUE_SIZE 0x10

struct pending_packet {
	struct list_head link;
	struct sk_buff *skb;
	struct sys_timer *timer;
};

OBJALLOC_DEF(arp_queue_pool, struct pending_packet, MAX_QUEUE_SIZE);

static LIST_HEAD(arp_queue);

static inline void set_ready(int arp_queue_info) {
	arp_queue_info |= 1;
}

static inline void set_transmitted(int arp_queue_info) {
	arp_queue_info |= (1 << 8);
}

static inline void set_answer(int arp_queue_info, int answer) {
	arp_queue_info = (answer << 16);
}

void arp_queue_process(struct sk_buff *arp_pack) {
	struct pending_packet *pack, *n;

	list_for_each_entry_safe(pack, n, &arp_queue, link) {
		if(arp_pack->nh.arph->ar_sip == pack->skb->nh.iph->daddr) {
			timer_close(pack->timer); //TODO it must be synchronized with drop function

			list_del(&pack->link);

			set_ready(pack->skb->sk->sk_deferred_info);
			set_answer(pack->skb->sk->sk_deferred_info, dev_queue_xmit(pack->skb));
			set_transmitted(pack->skb->sk->sk_deferred_info);

			pool_free(&arp_queue_pool, pack);
		}
	}
}

static void arp_queue_drop(struct sys_timer *timer, void *data) {
	struct pending_packet *deff_pack;
	net_device_stats_t *stats;
	struct net_device *dev;

	/* it must send message, that packet was dropped */
	deff_pack = (struct pending_packet*) data;
	kfree_skb(deff_pack->skb);
	dev = deff_pack->skb->dev;
	stats = dev->netdev_ops->ndo_get_stats(dev);
	stats->tx_err++;

	timer_close(timer);
	list_del(&deff_pack->link);
	pool_free(&arp_queue_pool, deff_pack);

	deff_pack->skb->sk->sk_deferred_info |= 1;
}

int arp_queue_add(struct sk_buff *skb) {
	struct sys_timer *timer;
	struct pending_packet *queue_pack;

	if(NULL == (queue_pack = (struct pending_packet*)pool_alloc(&arp_queue_pool))) {
		return -ENOMEM;
	}
	skb->sk->sk_deferred_info &= ~1;

	timer_set(&timer, TTL, arp_queue_drop, queue_pack);
	queue_pack->timer = timer;

	queue_pack->skb = skb;

	list_add_tail(&arp_queue, &queue_pack->link);

	return ENOERR;
}
