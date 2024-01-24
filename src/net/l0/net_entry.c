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

#include <lib/libds/dlist.h>

#include <hal/ipl.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/l0/net_rx.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/lthread/lthread.h>

#define NETIF_RX_HND_PRIORITY OPTION_GET(NUMBER, hnd_priority)

static DLIST_DEFINE(netif_rx_list);

static int netif_rx_action(struct lthread *self);
static LTHREAD_DEF(netif_rx_irq_handler, netif_rx_action, NETIF_RX_HND_PRIORITY);

static int netif_tx_action(struct lthread *self);
static LTHREAD_DEF(netif_tx_handler, netif_tx_action, NETIF_RX_HND_PRIORITY);

static int netif_rx_action(struct lthread *self) {
	struct net_device *dev = NULL;
	ipl_t ipl;

	ipl= ipl_save();
	{
		dlist_foreach_entry_safe(dev, &netif_rx_list, rx_lnk) {
			struct sk_buff *skb;

			while ((skb = skb_queue_pop(&dev->dev_queue)) != NULL) {
				ipl_restore(ipl);
				{
					net_rx(skb);
				}
				ipl= ipl_save();
			}
			dlist_del_init(&dev->rx_lnk);
		}
	}
	ipl_restore(ipl);

	return 0;
}

/* we can be in irq mode */
int netif_rx(void *data) {
	ipl_t ipl;
	struct sk_buff *skb = data;
	struct net_device *dev;

	assert(skb != NULL);
	assert(skb->dev != NULL);

	dev = skb->dev;

	ipl = ipl_save();
	{
		skb_queue_push(&dev->dev_queue, skb);

		if (dlist_empty(&dev->rx_lnk)) {
			dlist_add_prev(&dev->rx_lnk, &netif_rx_list);
		}

		lthread_launch(&netif_rx_irq_handler);
	}
	ipl_restore(ipl);

	return NET_RX_SUCCESS;
}

static DLIST_DEFINE(netif_tx_list);

static struct lthread netif_tx_handler;

static int netif_tx_action(struct lthread *self) {
	struct net_device *dev = NULL;

	sched_lock();
	{
		dlist_foreach_entry_safe(dev, &netif_tx_list, tx_lnk) {
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
