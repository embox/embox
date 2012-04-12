/**
 * @file
 * @brief
 *
 * @date 23.03.12
 * @author Alexander Kalmuk
 */

#include <embox/unit.h>
#include <assert.h>
#include <embox/unit.h>

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/repo.h>
#include <pnet/pnet_pack.h>

#include <net/skbuff.h>
#include <net/netdevice.h>

EMBOX_UNIT_INIT(init);

static int net_dev_rx_hnd(struct pnet_pack *pack) {
	struct sk_buff *skb;
	net_node_t node;

	skb = (struct sk_buff *) pack->data;

	node = pnet_get_dev_by_device(skb->dev);
	assert(node);

	if (node->graph) {
		pack->node = node;
	} else {
		pack->node = pnet_get_module("devs entry");
	}

	return 0;
}

PNET_NODE_DEF("devs resolver", {
	.rx_hnd = net_dev_rx_hnd,
	.tx_hnd = NULL,
});

static int init(void) {
	net_node_t entry, resolver;

	entry = pnet_get_module("pnet entry");
	resolver = pnet_get_module("devs resolver");
	entry->rx_dfault = resolver;

	return 0;
}
