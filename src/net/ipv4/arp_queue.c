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

#include <kernel/thread/api.h>
#include <kernel/thread/event.h>
#include <util/debug_msg.h>

#define TTL 1000
#define ARP_QUEUE_SIZE 0x10

struct pending_packet {
	struct list_head link;
	struct sk_buff *skb;
	struct sys_timer *timer;
};

OBJALLOC_DEF(arp_queue_pool, struct pending_packet, ARP_QUEUE_SIZE);
static LIST_HEAD(arp_queue);


void arp_queue_process(struct sk_buff *arp_pack) {
	struct pending_packet *pack, *safe;

	list_for_each_entry_safe(pack, safe, &arp_queue, link) {
		if(arp_pack->nh.arph->ar_sip == pack->skb->nh.iph->daddr) {
			timer_close(pack->timer); //TODO it must be synchronized with drop function

			list_del(&pack->link);

			sock_set_ready(pack->skb->sk);
			debug_printf("waking up socket", "arp_queue", "arp_queue_process");
			event_fire(&pack->skb->sk->sock_is_ready);
			dev_queue_xmit(pack->skb);


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
	dev = deff_pack->skb->dev;
	stats = dev->netdev_ops->ndo_get_stats(dev);
	stats->tx_err++;

	timer_close(timer);
	list_del(&deff_pack->link);

	sock_set_ready(deff_pack->skb->sk);
	debug_printf("launching socket event", "arp_queue", "arp_queue_drop");
	event_fire(&deff_pack->skb->sk->sock_is_ready);

	kfree_skb(deff_pack->skb);
	pool_free(&arp_queue_pool, deff_pack);
}

int arp_queue_add(struct sk_buff *skb) {
	struct sys_timer *timer;
	struct pending_packet *queue_pack;
//	struct sk_buff *new_pack;

	queue_pack = (struct pending_packet*)pool_alloc(&arp_queue_pool);
	if (queue_pack == NULL) {
		return -ENOMEM;
	}
	/* skb->sk->arp_queue_info = 0; */
	sock_unset_ready(skb->sk);

	timer_set(&timer, TTL, arp_queue_drop, queue_pack);
	queue_pack->timer = timer;

	queue_pack->skb = skb;

	list_add_tail(&arp_queue, &queue_pack->link);

	return ENOERR;
}
