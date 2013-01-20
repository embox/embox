/**
 * @file
 * @brief
 *
 * @date 27.10.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <assert.h>

#include <stdio.h>
int netif_rx(void *data) {
	struct sk_buff *skb;

	printf("netif_rx(): data %p\n", data);

	skb = (struct sk_buff *)data;
	assert(skb != NULL);

	//TODO move to processing
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;

	//skb_queue_push(&(dev->dev_queue), skb);
	netif_rx_schedule(skb);

	return NET_RX_SUCCESS;
}
