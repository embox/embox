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
#include <errno.h>
#include <net/sock.h>

#include <framework/net/pack/api.h>

EMBOX_UNIT_INIT(unit_init);

/*FIXME we must have queue for each netdevice or if we want to use the only
 we should use alloc_skb_queue?*/
//static SKB_LIST_HEAD(netdev_skb_head);


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
	int res;
	net_device_t *dev;
	const struct net_device_ops *ops;
	net_device_stats_t *stats;

	if (skb == NULL) {
		return -EINVAL;
	}

	dev = skb->dev;
	if (NULL == dev) {
		kfree_skb(skb);
		return -EINVAL;
	}

	ops = dev->netdev_ops;
	stats = ops->ndo_get_stats(dev);
	if (dev->flags & IFF_UP) {
		res = dev->header_ops->rebuild(skb);
		if (res < 0) {
			while(!skb->sk->is_ready);
			if(skb->sk->answer < 0) {
				kfree_skb(skb);
				stats->tx_err++;
				return res;
			} else
				return skb->sk->answer;
		}
		res = ops->ndo_start_xmit(skb, dev);
		if (res < 0) {
			kfree_skb(skb);
			stats->tx_err++;
			return res;
		}
		/* update statistic */
		stats->tx_packets++;
		stats->tx_bytes += skb->len;
	}
	return ENOERR;
}

int __netif_rx(struct sk_buff *skb) {
	net_device_t *dev;
	struct packet_type *q = NULL;
	const struct net_pack *pack;

	if (NULL == skb) {
		return NET_RX_DROP;
	}
	dev = skb->dev;
	if (NULL == dev) {
		kfree_skb(skb);
		return NET_RX_DROP;
	}
	skb->nh.raw = (unsigned char *) skb->data + ETH_HEADER_SIZE;

	net_pack_foreach(pack) {
		q = pack->netpack;
		if (q->type == skb->protocol) {
			skb_queue_tail(&(dev->dev_queue), skb);
			netif_rx_schedule(dev);
			return NET_RX_SUCCESS;
		}
	}
	kfree_skb(skb);
	return NET_RX_DROP;
}

int __netif_receive_skb(sk_buff_t *skb) {
	struct packet_type *q;
	const struct net_pack *pack;

	net_pack_foreach(pack) {
		q = pack->netpack;
		if (q->type == skb->protocol) {
			return q->func(skb, skb->dev, q, NULL);
		}
	}

#if 0

	net_pack_foreach(
	list_for_each_entry(q, &ptype_base, list) {
		if (q->type == skb->protocol) {
			return q->func(skb, skb->dev, q, NULL);
		}
	}
#endif
	kfree_skb(skb);
	return NET_RX_DROP;
}


void netif_rx_schedule(net_device_t *dev) {
	//TODO:
	raise_softirq(NET_RX_SOFTIRQ);
}

extern net_device_t *get_dev_by_idx(int num); /* TODO delete it */

static void net_rx_action(struct softirq_action *action) {
	size_t i;
	net_device_t *dev;
	for (i = 0; i < CONFIG_NET_DEVICES_QUANTITY; i++) {
		dev = get_dev_by_idx(i);
		if (dev) {
			dev->poll(dev);
		}
	}
}

static int __init unit_init(void) {
	open_softirq(NET_RX_SOFTIRQ, net_rx_action, NULL);
	return 0;
}
