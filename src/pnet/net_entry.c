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

struct sk_buff_head *pnet_queue;

struct pnet_desc {
	struct list_head lnk;
	struct sk_buff *skb;
};

int pnet_entry(struct pnet_pack *pack) {
	net_device_t *dev = pnet_get_net_device(pack->node);
	pack->skbuf->dev = dev;

	netif_rx(pack->skbuf);

	pnet_pack_destroy(pack);

	return 0;
}

static void pnetif_rx_schedule(struct sk_buff *skb) {
	skb_queue_tail(pnet_queue, skb);

	raise_softirq(PNET_RX_SOFTIRQ);
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

	// skb->nh.raw = (unsigned char *) skb->data + ETH_HEADER_SIZE;

	pnetif_rx_schedule(skb);
	return NET_RX_SUCCESS;
}

static void pnet_rx_action(struct softirq_action *action) {
	net_node_t node;
	struct sk_buff *skb;
	struct pnet_pack *pack;

	while(NULL != (skb = skb_dequeue(pnet_queue))) {
		node = pnet_get_module("matcher");
		pack = pnet_pack_create(skb, skb->len, PNET_PACK_TYPE_SKB);
		pack->node = node;
		pack->skbuf = skb;
		if(!(MATCH_SUCCESS == match(pack))) {
			netif_rx_schedule(skb->dev);
		}
	}
}

static int unit_init(void) {
	pnet_queue = alloc_skb_queue();
	open_softirq(PNET_RX_SOFTIRQ, pnet_rx_action, &pnet_queue);
	return 0;
}
