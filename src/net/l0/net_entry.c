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
#include <embox/unit.h>
#include <hal/ipl.h>
#include <linux/interrupt.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <util/list.h>
#include <net/l0/net_rx.h>

EMBOX_UNIT_INIT(net_entry_init);

static LIST_DEF(netif_rx_list);

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


static void netif_rx_action(struct softirq_action *action) {
	struct net_device *dev;

	list_foreach(dev, &netif_rx_list, rx_lnk) {
		netif_poll(dev);
		netif_rx_dequeued(dev);
	}
}

static void netif_rx_schedule(struct sk_buff *skb) {
	struct net_device *dev;

	assert(skb != NULL);

	dev = skb->dev;
	assert(dev != NULL);

	skb_queue_push(&dev->dev_queue, skb);

	netif_rx_queued(dev);

	raise_softirq(NET_RX_SOFTIRQ);
}

int netif_rx(void *data) {
	assert(data != NULL);
	netif_rx_schedule((struct sk_buff *)data);
	return NET_RX_SUCCESS;
}

static int net_entry_init(void) {
	open_softirq(NET_RX_SOFTIRQ, netif_rx_action, NULL);
	return 0;
}
