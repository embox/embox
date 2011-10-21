/**
 * @file
 *
 * @brief
 *
 * @date 19.10.2011
 * @author Anton Bondarev
 */

#if 0
#include <net/in.h>
#include <pnet/prior_path.h>
#include <kernel/thread/api.h>
#include <net/skbuff.h>

int netif_rx(struct sk_buff skb) {
	netif_add_rx_pack(skb);
	softirq_fire();

	return 0;
}

//static struct thread *pnet_rx_threads[0x20];
//TODO move from here
/** get next received packet from common rx queue */
static struct sk_buff *netif_get_rx_pack(void) {
	return NULL;
}

int netif_rx_dispatch(void) {
	struct pnet_path *prior_path;
	struct sk_buff *skb;

	while(NULL != (skb = netif_get_rx_pack())) {
		prior_path = pnet_get_dev_prior(skb->dev);
	}

	return 0;
}
#endif
