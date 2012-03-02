/**
 * @file
 * @brief
 *
 * @date 27.10.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <pnet/prior_path.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>

#include <linux/interrupt.h>
#include <embox/unit.h>
#include <linux/init.h>
#include <pnet/match.h>
#include <pnet/pnet_pack.h>

EMBOX_UNIT_INIT(unit_init);

int pnet_entry(struct pnet_pack *pack) {
	net_device_t *dev = pnet_get_net_device(pack->node);
	pack->skbuf->dev = dev;

	netif_rx(pack->skbuf);

	pnet_pack_destroy(pack);

	return 0;
}

int pnetif_rx(struct sk_buff *skb) {
	net_device_t *dev;

	if (NULL == skb) {
		return NET_RX_DROP;
	}

	dev = skb->dev;

	if (NULL == dev) {
		kfree_skb(skb);
		return NET_RX_DROP;
	}

	skb->nh.raw = (unsigned char *) skb->data + ETH_HEADER_SIZE;
	skb_queue_tail(&(dev->dev_queue), skb);
	netif_rx_schedule(dev);
	return NET_RX_SUCCESS;
}
#if 0
int netif_receive_skb(sk_buff_t *skb) {
	struct pnet_pack *pack;
	net_node_t node;

	node = pnet_get_dev_by_device(skb->dev);
	//pnet pack = pnet_pack_alloc_skb(node->rx_dfault, skb);
//	pack = NULL;
	pnet_rx_thread_add(skb, node);

	return 0;
}
#endif

void pnetif_rx_schedule(void) {
	raise_softirq(PNET_RX_SOFTIRQ);
}

static int is_empty_pnet(void) {
	return 1;
}

static int is_empty_net(void) {
	return 0;
}

static int match_skb(struct sk_buff *skb, net_node_t node) {
	return 0;
}

static void pnet_rx_action(struct softirq_action *action) {
	net_node_t node;
	struct sk_buff *skb;

	skb = skb_dequeue(NULL);
	while(!is_empty_pnet()) {
		//match(pack);
	}
	while(!is_empty_net()) {
		node = pnet_get_dev_by_device(skb->dev);
		if(!match_skb(skb, node)) {
			netif_rx_schedule(skb->dev);
		}
	}


}

static int unit_init(void) {
	open_softirq(PNET_RX_SOFTIRQ, pnet_rx_action, NULL);
	return 0;
}
