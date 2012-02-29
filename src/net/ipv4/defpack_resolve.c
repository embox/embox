/**
 * @brief The queue of packets waiting ARP resolving
 *
 * @date 13.12.2011
 * @author Alexander Kalmuk
 */

#include <types.h>
#include <util/list.h>
#include <mem/objalloc.h>
#include <errno.h>

#include <net/skbuff.h>
#include <net/ip.h>
#include <net/inetdevice.h>
#include <net/arp.h>

#include <kernel/timer.h>

#include <net/defpack_resolve.h>

struct deff_packet {
	struct list_head link;
	struct sk_buff *skb;
	struct sys_timer *timer;
};

OBJALLOC_DEF(arp_qeueue_pool, struct deff_packet, MAX_PACKET_CNT);

static LIST_HEAD(deff_packet_list);

void def_process_list(struct sk_buff *arp_pack) {
	struct deff_packet *pack, *n;

	list_for_each_entry_safe(pack, n, &deff_packet_list, link) {
		if(arp_pack->nh.arph->ar_sip == pack->skb->nh.iph->daddr) {
			timer_close(pack->timer); //TODO it must be synchronized with drop function

			list_del(&pack->link);

			pack->skb->sk->sk_deferred_info |= (1 << 8);
			pack->skb->sk->sk_deferred_info |= 1;
			pack->skb->sk->sk_deferred_info |= (dev_queue_xmit(pack->skb) << 16);

			pool_free(&arp_qeueue_pool, pack);
		}
	}
}

static void arp_queue_drop(struct sys_timer *timer, void *data) {
	struct deff_packet *deff_pack;
	net_device_stats_t *stats;
	struct net_device *dev;

	/* it must send message, that packet was dropped */
	deff_pack = (struct deff_packet*) data;
	kfree_skb(deff_pack->skb);
	dev = deff_pack->skb->dev;
	stats = dev->netdev_ops->ndo_get_stats(dev);
	stats->tx_err++;

	timer_close(timer);
	list_del(&deff_pack->link);
	pool_free(&arp_qeueue_pool, deff_pack);

	deff_pack->skb->sk->sk_deferred_info |= 1;
}

int def_add_packet(struct sk_buff *skb) {
	struct sys_timer *timer;
	struct deff_packet *queue_pack;

	if(NULL == (queue_pack = (struct deff_packet*)pool_alloc(&arp_qeueue_pool))) {
		return -ENOMEM;
	}
	skb->sk->sk_deferred_info &= ~1;

	timer_set(&timer, DEF_PACKET_TTL, arp_queue_drop, queue_pack);
	queue_pack->timer = timer;

	queue_pack->skb = skb;

	list_add_tail(&deff_packet_list, &queue_pack->link);

	return ENOERR;
}
