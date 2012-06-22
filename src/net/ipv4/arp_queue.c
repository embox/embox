/**
 * @file
 * @brief Handle queue of packets waiting for ARP resolving.
 *
 * @date 13.12.11
 * @author Alexander Kalmuk
 */

#include <net/arp_queue.h>
#include <types.h>
#include <util/list.h>
#include <mem/objalloc.h>
#include <errno.h>
#include <net/skbuff.h>
#include <kernel/thread/event.h>
#include <kernel/timer.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/arp.h>
#include <assert.h>

#define TTL 1000
#define ARP_QUEUE_SIZE 0x10

struct pending_packet {
	struct list_head link;
	struct sk_buff *skb;
	struct sys_timer *timer;
};

OBJALLOC_DEF(arp_queue_allocator, struct pending_packet, ARP_QUEUE_SIZE);
static LIST_HEAD(arp_queue);

void arp_queue_process(struct sk_buff *arp_pack) {
	struct pending_packet *pack, *safe;
	struct sk_buff *skb;

	assert(arp_pack != NULL);

	list_for_each_entry_safe(pack, safe, &arp_queue, link) {
		if (arp_pack->nh.arph->ar_sip == pack->skb->nh.iph->daddr) {
			skb = pack->skb;
			assert(skb != NULL);

			timer_close(pack->timer); //TODO it must be synchronized with drop function
			list_del(&pack->link);
			objfree(&arp_queue_allocator, pack);

			sock_set_ready(skb->sk);
			event_fire(&skb->sk->sock_is_ready);
			dev_queue_xmit(skb);
		}
	}
}

static void arp_queue_drop(struct sys_timer *timer, void *data) {
	int res;
	struct pending_packet *deff_pack;
	net_device_stats_t *stats;
	struct net_device *dev;

	/* it must send message, that packet was dropped */
	assert((timer != NULL) && (data != NULL));

	deff_pack = (struct pending_packet *)data;

	assert(deff_pack->skb != NULL);
	assert(deff_pack->skb->dev != NULL);

	dev = deff_pack->skb->dev;

	assert(dev->netdev_ops != NULL);
	assert(dev->netdev_ops->ndo_get_stats != NULL);
	stats = dev->netdev_ops->ndo_get_stats(dev);

	stats->tx_err++;

	res = timer_close(timer);
	assert(res == ENOERR);

	list_del(&deff_pack->link);

	sock_set_ready(deff_pack->skb->sk);
	event_fire(&deff_pack->skb->sk->sock_is_ready);

	kfree_skb(deff_pack->skb);
	objfree(&arp_queue_allocator, deff_pack);
}

int arp_queue_add(struct sk_buff *skb) {
	int res;
	struct pending_packet *queue_pack;

	assert(skb != NULL);

	queue_pack = (struct pending_packet *)objalloc(&arp_queue_allocator);
	if (queue_pack == NULL) {
		return -ENOMEM;
	}

	queue_pack->skb = skb;

	res = timer_set(&queue_pack->timer, TIMER_ONESHOT, TTL, arp_queue_drop, queue_pack);
	if (res < 0) {
		objfree(&arp_queue_allocator, queue_pack);
		return res;
	}

	list_add_tail(&queue_pack->link, &arp_queue);
	/* skb->sk->arp_queue_info = 0; */
	sock_unset_ready(skb->sk);

	return ENOERR;
}
