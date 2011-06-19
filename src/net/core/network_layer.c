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
#include <kernel/printk.h>
#include <linux/list.h>
#include <linux/init.h>
#include <net/inetdevice.h>
#include <kernel/irq.h>
#include <linux/interrupt.h>

extern int dev_is_busy(int num);

EMBOX_UNIT_INIT(unit_init);

/*FIXME we must have queue for each netdevice or if we want to use the only
 we should use alloc_skb_queue?*/
//static SKB_LIST_HEAD(netdev_skb_head);

static LIST_HEAD(ptype_base);
static LIST_HEAD(ptype_all);


/* we use this function in debug mode*/
#if 0
static void print_packet (sk_buff_t *skb) {
	size_t i, j;
	printf("pack:\n");
	for (i = 0; i < skb->len; i += 16) {
		for (j = 0; j < 16; j += 2) {
			printf("%02x%02x ",
				(uint8_t)skb->data[i + j],
				(uint8_t)skb->data[i + j + 1]);
		}
		printf("\n");
	}
	printf("\n");
	return;
}
#endif

int dev_queue_xmit(struct sk_buff *skb) {
	net_device_t *dev;
	const struct net_device_ops *ops;
	net_device_stats_t *stats;
	if (NULL == skb) {
		return -1;
	}
	dev = skb->dev;
	if (NULL == dev) {
		return -1;
	}
	ops = dev->netdev_ops;
	stats = ops->ndo_get_stats(dev);
	printf("\ndev161.try send\n");
	if (dev->flags & IFF_UP) {
		if (ETH_P_ARP != skb->protocol) {
			if (-1 == dev->header_ops->rebuild(skb)) {
				kfree_skb(skb);
				stats->tx_err++;
				return -1;
			}
		}
		if (-1 == ops->ndo_start_xmit(skb, dev)) {
			kfree_skb(skb);
			stats->tx_err++;
			return -1;
		}
		/* update statistic */
		stats->tx_packets++;
		stats->tx_bytes += skb->len;
	}
	kfree_skb(skb);
	return 0;
}


int netif_rx(struct sk_buff *skb) {
	net_device_t *dev;
	struct packet_type *q = NULL;
	int rc_rx = 0;

	if (NULL == skb) {
		return NET_RX_DROP;
	}
	dev = skb->dev;
	if (NULL == dev) {
		return NET_RX_DROP;
	}
	skb->nh.raw = (unsigned char *) skb->data + ETH_HEADER_SIZE;

	list_for_each_entry(q, &ptype_base, list) {
		if (q->type == skb->protocol) {
				skb_queue_tail(&(dev->dev_queue), skb);
				netif_rx_schedule(dev);
			return rc_rx;
		}
	}
	kfree_skb(skb);
	return NET_RX_DROP;
}

int netif_receive_skb(sk_buff_t *skb) {
	struct packet_type *q;
	list_for_each_entry(q, &ptype_base, list) {
		if (q->type == skb->protocol) {
			q->func(skb, skb->dev, q, NULL);
			return 0;
		}
	}
	kfree_skb(skb);
	return 0;
}


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

void netif_rx_schedule(net_device_t *dev) {
	//TODO:
	raise_softirq(NET_RX_SOFTIRQ);
}
extern net_device_t *get_dev_by_idx(int num);
static void net_rx_action(struct softirq_action *action) {
	size_t i;
	net_device_t *dev;
	for (i = 0; i < CONFIG_NET_DEVICES_QUANTITY; i++) {
		if (dev_is_busy(i)) {
			dev = get_dev_by_idx(i);
			dev->poll(dev);
		}
	}
}

static int __init unit_init(void) {
	open_softirq(NET_RX_SOFTIRQ, net_rx_action, NULL);
	return 0;
}
