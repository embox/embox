/**
 * @file
 * @brief
 *
 * @date 19.06.11
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <framework/net/pack/api.h>
#include <net/netdevice.h>
#include <net/skbuff.h>

int netif_receive_skb(struct sk_buff *skb) {
	const struct net_pack *npack;

	assert(skb != NULL);

	npack = net_pack_lookup(skb->protocol);
	if (npack == NULL) {
		skb_free(skb);
		return 0;
	}

	return npack->handle(skb, skb->dev);
}
