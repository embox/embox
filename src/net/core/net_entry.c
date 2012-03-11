/**
 * @file
 * @brief
 *
 * @date 27.10.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <mem/misc/pool.h>
#include <net/if.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/interrupt.h>

#include <embox/unit.h>

int netif_rx(struct sk_buff *skb) {
	struct net_device *dev;

	if (NULL == skb) {
		return NET_RX_DROP;
	}
	dev = skb->dev;
	if (NULL == dev) {
		kfree_skb(skb);
		return NET_RX_DROP;
	}
	//TODO move to processing
	skb->nh.raw = (unsigned char *) skb->data + ETH_HEADER_SIZE;

	//skb_queue_tail(&(dev->dev_queue), skb);
	netif_rx_schedule(skb);

	return NET_RX_DROP;
}

