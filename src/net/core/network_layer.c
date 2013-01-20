/**
 * @file
 *
 * @brief
 *
 * @date 19.06.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <string.h>
#include <stdio.h>
#include <embox/unit.h>
#include <lib/list.h>
#include <net/inetdevice.h>
#include <linux/interrupt.h>
#include <kernel/irq.h>
#include <errno.h>
#include <net/sock.h>
#include <net/route.h>
#include <net/arp_queue.h>
#include <framework/net/pack/api.h>

EMBOX_UNIT_INIT(unit_init);

/* FIXME network packet's type is L2 (device layer protocols)
 * what is dev_add_pack, why was separated ptype_base and ptype_all
 * what this code does here?
 */
/*paket's types*/
static LIST_HEAD(ptype_base);
static LIST_HEAD(ptype_all);

void dev_add_pack(struct packet_type *pt) {
	if (pt->type == htons(ETH_P_ALL)) {
		list_add(&pt->list, &ptype_all);
	} else {
		list_add(&pt->list, &ptype_base);
	}
}

void dev_remove_pack(struct packet_type *pt) {
	struct list_head *head;
	struct packet_type *pt1;

	if (pt->type == htons(ETH_P_ALL)) {
		head = &ptype_all;
	} else {
		head = &ptype_base;
	}
	list_for_each_entry(pt1, head, list) {
		if (pt == pt1) {
			list_del(&pt->list);
			return;
		}
	}
}

#ifdef DEBUG
/* we use this function in debug mode*/
static void print_packet (sk_buff_t *skb) {
	size_t i, j;
	printf("pack:\n");
	for (i = 0; i < skb->len; i += 16) {
		for (j = 0; j < 16; j += 2) {
			printf("%02x%02x ",
				(uint8_t)skb->mac.raw[i + j],
				(uint8_t)skb->mac.raw[i + j + 1]);
		}
		printf("\n");
	}
	printf("\n");
	return;
}
#endif

int dev_queue_send(struct sk_buff *skb) {
	int res;

	printf("dev_queue_send() init skb %p\n", skb);

	assert(skb != NULL);
	assert(skb->dev != NULL);
	assert(skb->dev->header_ops != NULL);
	assert(skb->dev->header_ops->rebuild != NULL);

	printf("dev_queue_send() rebuild skb %p\n", skb);
	res = skb->dev->header_ops->rebuild(skb);
	if (res < 0) {
		/* send arp request and add packet in list of deferred packets */
		printf("dev_queue_send() save skb %p to arp_queue\n", skb);
		res = arp_queue_add(skb);
		printf("dev_queue_send() arp_queue_add return %d for %p\n", res, skb);
		if (res < 0) {
			skb_free(skb);
			return res;
		}
		return -EINPROGRESS;
#if 0
		if (sock_is_ready(skb->sk)) {
			/* If socket is ready then it was really error
			 * but if socket isn't ready package has been saved
			 * and will be transmitted later */
			skb_free(skb);
		}

		return res;
#endif
	}

	printf("dev_queue_send() send skb %p\n", skb);
	return dev_queue_xmit(skb);
}

int dev_queue_xmit(struct sk_buff *skb) {
	int res;
	net_device_t *dev;
	const struct net_device_ops *ops;
	net_device_stats_t *stats;

	printf("dev_queue_xmit() init skb %p\n", skb);

	assert(skb != NULL);

	dev = skb->dev;
	assert(dev != NULL);

	ops = dev->netdev_ops;
	assert(ops != NULL);

	assert(ops->ndo_get_stats != NULL);
	stats = ops->ndo_get_stats(dev);
	assert(stats != NULL);

	if (dev->flags & IFF_UP) {
		printf("dev_queue_xmit() xmit skb %p\n", skb);
		assert(ops->ndo_start_xmit != NULL);
		res = ops->ndo_start_xmit(skb, dev);
		printf("dev_queue_xmit() xmit return %d for skb %p\n", res, skb);
		if (res < 0) {
			skb_free(skb);
			stats->tx_err++;
			return res;
		}

		/* update statistic */
		stats->tx_packets++;
		stats->tx_bytes += skb->len;
	}
	else {
		skb_free(skb);
	}

	printf("dev_queue_xmit() fini skb %p\n", skb);

	return ENOERR;
}

#include <stdio.h>
int netif_receive_skb(sk_buff_t *skb) {
	struct packet_type *q;
	const struct net_pack *pack;

	assert(skb != NULL);

	printf("netif_receive_skb() receive skb %p\n", skb);

	net_pack_foreach(pack) {
		assert(pack != NULL);

		q = pack->netpack;
		assert(q != NULL);

		if (q->type == skb->protocol) {
			assert(q->func != NULL);
			return q->func(skb, skb->dev, q, NULL);
		}
	}

	skb_free(skb);
	return NET_RX_DROP;
}

static void net_rx_action(struct softirq_action *action) {
	netdev_rx_processing();
}


void netif_rx_schedule(struct sk_buff *skb) {
	struct net_device *dev;

	assert(skb != NULL);

	dev = skb->dev;
	assert(dev != NULL);

	skb_queue_push(&(dev->dev_queue), skb);

	netdev_rx_queued(dev);

	raise_softirq(NET_RX_SOFTIRQ);
}

static int unit_init(void) {
	open_softirq(NET_RX_SOFTIRQ, net_rx_action, NULL);
	return ENOERR;
}
