/**
 * @file
 * @brief
 *
 * @date 27.10.11
 * @author Anton Kozlov
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */
#include <util/log.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <util/dlist.h>

#include <hal/ipl.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/l0/net_rx.h>
#include <embox/unit.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/lthread/lthread.h>

#define NETIF_RX_HND_PRIORITY OPTION_GET(NUMBER, hnd_priority)

EMBOX_UNIT_INIT(net_entry_init);

static DLIST_DEFINE(netif_rx_list);

static struct lthread netif_rx_irq_handler;

static void netif_rx_queued(struct net_device *dev) {
	ipl_t sp;

	assert(dev != NULL);

	sp = ipl_save();
	{
		if (dlist_empty(&dev->rx_lnk)) {
			dlist_add_prev(&dev->rx_lnk, &netif_rx_list);
		}
	}
	ipl_restore(sp);
}

static void netif_rx_dequeued(struct net_device *dev) {
	ipl_t sp;

	assert(dev != NULL);

	sp = ipl_save();
	{
		assert(!dlist_empty(&dev->rx_lnk));
		dlist_del_init(&dev->rx_lnk);
	}
	ipl_restore(sp);
}

static void netif_poll(struct net_device *dev) {
	struct sk_buff *skb;

	while ((skb = skb_queue_pop(&dev->dev_queue)) != NULL) {
		net_rx(skb);
	}
}


static int netif_rx_action(struct lthread *self) {
	struct net_device *dev;

	dlist_foreach_entry(dev, &netif_rx_list, rx_lnk) {
		netif_poll(dev);
		netif_rx_dequeued(dev);
	}

	return 0;
}

static void netif_rx_schedule(struct sk_buff *skb) {
	struct net_device *dev;

	assert(skb != NULL);

	dev = skb->dev;
	assert(dev != NULL);

	skb_queue_push(&dev->dev_queue, skb);

	netif_rx_queued(dev);

	lthread_launch(&netif_rx_irq_handler);
}

int netif_rx(void *data) {
	assert(data != NULL);
	netif_rx_schedule((struct sk_buff *) data);
	return NET_RX_SUCCESS;
}

static DLIST_DEFINE(netif_tx_list);

static struct lthread netif_tx_handler;

static int netif_tx_action(struct lthread *self) {
	struct net_device *dev;

	sched_lock();
	{
		dlist_foreach_entry(dev, &netif_tx_list, tx_lnk) {
			struct sk_buff * skb;
			int ret;

			while ((skb = skb_queue_pop(&dev->dev_queue_tx)) != NULL) {
				assert(dev->drv_ops != NULL);
				assert(dev->drv_ops->xmit != NULL);
				ret = dev->drv_ops->xmit(dev, skb);
				if (ret != 0) {
					log_debug("xmit = %d", ret);
					skb_free(skb);
					dev->stats.tx_err++;
					continue;
				}

				dev->stats.tx_packets++;
				dev->stats.tx_bytes += skb->len;
			}

			dlist_del_init(&dev->tx_lnk);
		}
	}
	sched_unlock();


	return 0;
}

int netif_tx(struct net_device *dev,  struct sk_buff *skb) {
	sched_lock();
	{
		skb_queue_push(&dev->dev_queue_tx, skb);

		if (dlist_empty(&dev->tx_lnk)) {
			dlist_add_prev(&dev->tx_lnk, &netif_tx_list);
		}
		lthread_launch(&netif_tx_handler);
	}
	sched_unlock();

	return 0;
}

static int net_entry_init(void) {
	lthread_init(&netif_rx_irq_handler, &netif_rx_action);
	schedee_priority_set(&netif_rx_irq_handler.schedee, NETIF_RX_HND_PRIORITY);

	lthread_init(&netif_tx_handler, &netif_tx_action);
	schedee_priority_set(&netif_tx_handler.schedee, NETIF_RX_HND_PRIORITY);

	return 0;
}
