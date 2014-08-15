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
#include <util/list.h>
#include <net/l0/net_rx.h>
#include <embox/unit.h>

#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_priority.h>
#include <err.h>

EMBOX_UNIT_INIT(net_entry_init);

static LIST_DEF(netif_rx_list);

static struct lthread *netif_rx_irq_handler;

static void netif_rx_queued(struct net_device *dev) {
	ipl_t sp;

	assert(dev != NULL);

	sp = ipl_save();
	{
		if (list_alone_link(&dev->rx_lnk)) {
			list_add_last_link(&dev->rx_lnk, &netif_rx_list);
		}
	}
	ipl_restore(sp);
}

static void netif_rx_dequeued(struct net_device *dev) {
	ipl_t sp;

	assert(dev != NULL);

	sp = ipl_save();
	{
		assert(!list_alone_link(&dev->rx_lnk));
		list_unlink_link(&dev->rx_lnk);
	}
	ipl_restore(sp);
}

static void netif_poll(struct net_device *dev) {
	struct sk_buff *skb;

	while ((skb = skb_queue_pop(&dev->dev_queue)) != NULL) {
		net_rx(skb);
	}
}

static void *netif_rx_action(void *data) {
	struct net_device *dev;

	list_foreach(dev, &netif_rx_list, rx_lnk) {
		netif_poll(dev);
		netif_rx_dequeued(dev);
	}

	return NULL;
}

static void netif_rx_schedule(struct sk_buff *skb) {
	struct net_device *dev;

	assert(skb != NULL);

	dev = skb->dev;
	assert(dev != NULL);

	skb_queue_push(&dev->dev_queue, skb);

	netif_rx_queued(dev);

	lthread_launch(netif_rx_irq_handler);
}

int netif_rx(void *data) {
	assert(data != NULL);
	netif_rx_schedule((struct sk_buff *) data);
	return NET_RX_SUCCESS;
}

static int net_entry_init(void) {
	netif_rx_irq_handler = lthread_create(&netif_rx_action, NULL);
	assert(!err(netif_rx_irq_handler));
	lthread_priority_set(netif_rx_irq_handler, LTHREAD_PRIORITY_MAX);
	return 0;
}
