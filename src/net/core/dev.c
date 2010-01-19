/**
 * @file dev.c
 *
 * @brief Protocol independent device support routines.
 * @date 04.03.2009
 * @author Anton Bondarev
 */
#include <types.h>
#include <string.h>
#include <common.h>
#include <autoconf.h>
#include <kernel/module.h>
#include <kernel/interrupt.h>
#include <net/skbuff.h>
#include <net/net.h>
#include <net/arp.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <lib/list.h>
#include <kernel/irq.h>

DECLARE_INIT("net_dev", net_dev_init, INIT_NET_LEVEL);

//FIXME we must have queue for each netdevice or if we want to use the only
// we should use alloc_skb_queue?
static SKB_LIST_HEAD(netdev_skb_head);

static LIST_HEAD(ptype_base);
static LIST_HEAD(ptype_all);

typedef struct _NET_DEV_INFO {
	net_device_t dev;
	int is_busy;
} NET_DEV_INFO;

static NET_DEV_INFO net_devices[NET_DEVICES_QUANTITY];

static inline int dev_is_busy(int num) {
	return net_devices[num].is_busy;
}

static inline net_device_t *dev_lock(int num) {
	net_devices[num].is_busy = 1;
	return &net_devices[num].dev;
}

static inline void dev_unlock(int num) {
	net_devices[num].is_busy = 0;
}

int dev_alloc_name(struct net_device *dev, const char *name) {
	return 0;
}

net_device_t *alloc_netdev(int sizeof_priv, const char *name, void(*setup)(
		net_device_t *)) {
	struct net_device *dev;
	char buff[IFNAMSIZ];
	int i;
	for (i = 0; i < NET_DEVICES_QUANTITY; i++) {
		if (!dev_is_busy(i)) {
			dev = dev_lock(i);
			setup(dev);
			sprintf(buff, name, i);
			strcpy(dev->name, buff);
			return dev;
		}
	}
	return NULL;
}

void free_netdev(net_device_t *dev) {
	int i;
	for (i = 0; i < NET_DEVICES_QUANTITY; i++) {
		if (dev == &net_devices[i].dev) {
			dev_unlock(i);
		}
	}
}

int register_netdev(struct net_device *dev) {
	//FIXME we must create queue for each device
	return 0;
}

void unregister_netdev(struct net_device *dev) {

}

net_device_t *netdev_get_by_name(const char *name) {
	int i;
	for (i = 0; i < NET_DEVICES_QUANTITY; i++) {
		if (dev_is_busy(i) && !strncmp(name, net_devices[i].dev.name, IFNAMSIZ)) {
			return &net_devices[i].dev;
		}
	}
	return NULL;
}

int dev_queue_xmit(struct sk_buff *pack) {
	net_device_t *dev;
	const struct net_device_ops *ops;
	net_device_stats_t *stats;
	if (NULL == pack) {
		return -1;
	}
	dev = pack->dev;
	if (NULL == dev) {
		return -1;
	}
	ops = dev->netdev_ops;
	stats = ops->ndo_get_stats(dev);
	if (dev->flags & IFF_UP) {
		if (ETH_P_ARP != pack->protocol) {
			if (-1 == dev->header_ops->rebuild(pack)) {
				arp_queue(pack);
				return -1;
			}
		}
		if (-1 == ops->ndo_start_xmit(pack, dev)) {
			kfree_skb(pack);
			stats->tx_err++;
			return -1;
		}
		/* update statistic */
		stats->tx_packets++;
		stats->tx_bytes += pack->len;
	}
	kfree_skb(pack);
	return 0;
}

/* we use this function in debug mode*/
#if 0
static void print_packet (sk_buff_t *skb) {
	int i;
	TRACE("pack: ");
	for (i = 0; i < skb->len; i ++) {
		TRACE("%2X", (uint8_t)skb->data[i]);
	}
	TRACE("\n");
	return;
}
#endif

int netif_rx(struct sk_buff *skb) {
	net_device_t *dev;
	struct packet_type *q;
	int rc_rx = 0;

	//	unsigned long sp = local_irq_save();

	if (NULL == skb) {
		return NET_RX_DROP;
	}
	dev = skb->dev;
	if (NULL == dev) {
		return NET_RX_DROP;
	}
	skb->nh.raw = (void *) skb->data + ETH_HEADER_SIZE;

	list_for_each_entry(q, &ptype_base, list) {
		if (q->type == skb->protocol) {
#if CONFIG(SOFT_IRQ)
			if (ETH_P_ARP != skb->protocol) {
				skb_queue_tail(&netdev_skb_head, skb);
				__raise_softirq_irqoff(NET_RX_SOFTIRQ);
			} else {
#endif
			rc_rx = q->func(skb, dev, q, NULL);
			kfree_skb(skb);
#if CONFIG(SOFT_IRQ)
		}
#endif
			//			local_irq_restore(sp);
			return rc_rx;
		}
	}
	kfree_skb(skb);
	//	local_irq_restore(sp);
	return NET_RX_DROP;
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

int dev_open(struct net_device *dev) {
	int ret = 0;
	const struct net_device_ops *ops;
	/* Is it already up? */
	if (dev->flags & IFF_UP) {
		return 0;
	}
	ops = dev->netdev_ops;
	dev->state |= __LINK_STATE_START;

	if (ops->ndo_open) {
		ret = ops->ndo_open(dev);
	} else {
		LOG_ERROR("can't find open function in net_device\n");
	}
	if (ret) {
		dev->state &= ~__LINK_STATE_START;
	} else {
		/* Set the flags. */
		//TODO: IFF_RUNNING sets not here
		dev->flags |= (IFF_UP | IFF_RUNNING);
	}
	return ret;
}

int dev_close(struct net_device *dev) {
	const struct net_device_ops *ops;
	if (!(dev->flags & IFF_UP)) {
		return 0;
	}
	ops = dev->netdev_ops;
	dev->state &= ~__LINK_STATE_START;

	if (ops->ndo_stop) {
		ops->ndo_stop(dev);
	} else {
		LOG_ERROR("ifdev down: can't find stop function in net_device with name\n");
	}
	/* Device is now down. */
	//TODO: IFF_RUNNING sets not here
	dev->flags &= ~(IFF_UP | IFF_RUNNING);
	return 0;
}

unsigned dev_get_flags(const struct net_device *dev) {
	//TODO: ...
	return dev->flags;
}

int dev_change_flags(struct net_device *dev, unsigned flags) {
	int ret;
	int old_flags = dev->flags;
	if ((old_flags ^ flags) & IFF_UP) {
		ret = ((old_flags & IFF_UP) ? dev_close : dev_open)(dev);
	}
	//TODO: ...
	dev->flags = dev->flags & flags;
	return ret;
}

#if CONFIG(SOFT_IRQ)
static void netif_rx_schedule(struct softirq_action* action) {
	struct sk_buff *skb;
	struct packet_type *q;
	while (NULL != (skb = skb_dequeue(&netdev_skb_head))) {
		list_for_each_entry(q, &ptype_base, list) {
			if (q->type == skb->protocol) {
				q->func(skb, skb->dev, q, NULL);
				break;
			}
		}
		kfree_skb(skb);
	}
}
#endif

static int __init net_dev_init(void) {
#if CONFIG(SOFT_IRQ)
	open_softirq(NET_RX_SOFTIRQ, netif_rx_schedule, NULL);
#endif
	return 0;
}
