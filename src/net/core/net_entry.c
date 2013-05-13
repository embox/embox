/**
 * @file
 * @brief
 *
 * @date 27.10.11
 * @author Anton Kozlov
 */

#include <assert.h>
#include <net/if_ether.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stddef.h>

int netif_rx(void *data) {
	struct sk_buff *skb;

	assert(data != NULL);

	skb = (struct sk_buff *)data;
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE; /* TODO move to processing */

	netif_rx_schedule(skb);

	return NET_RX_SUCCESS;
}
