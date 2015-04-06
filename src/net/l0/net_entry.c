/**
 * @file
 * @brief
 *
 * @date 27.10.11
 * @author Anton Kozlov
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <hal/ipl.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <util/dlist.h>
#include <net/l0/net_rx.h>
#include <embox/unit.h>

#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>

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

static int net_entry_init(void) {
	lthread_init(&netif_rx_irq_handler, &netif_rx_action);
	schedee_priority_set(&netif_rx_irq_handler.schedee, NETIF_RX_HND_PRIORITY);
	return 0;
}
