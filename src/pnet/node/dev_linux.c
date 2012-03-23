/**
 * @file
 * @brief
 *
 * @date 23.03.12
 * @author Alexander Kalmuk
 */

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/pnet_pack.h>

#include <net/skbuff.h>

static int net_dev_rx_hnd(struct pnet_pack *pack) {
	struct sk_buff *skb;
	net_node_t node;

	skb = (struct sk_buff *) pack->data;

	node = pnet_get_dev_by_device(skb->dev);
	assert(node);
	pack->node = node;

	return NET_HND_SUPPRESSED;
}

PNET_NODE_DEF("devs resolver", {
	.rx_hnd = net_dev_rx_hnd,
	.tx_hnd = NULL
});
