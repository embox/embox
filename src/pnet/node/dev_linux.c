/**
 * @file
 * @brief
 *
 * @date 23.03.12
 * @author Alexander Kalmuk
 * @author Anton Kozlov
 *         - pnet_get_dev_by_device()
 */

#include <embox/unit.h>
#include <assert.h>
#include <embox/unit.h>
#include <string.h>

#include <pnet/core/core.h>
#include <pnet/core/node.h>
#include <pnet/core/repo.h>
#include <pnet/pack/pnet_pack.h>
#include <pnet/core/graph.h>

#include <net/skbuff.h>
#include <net/netdevice.h>
#include <lib/libds/hashtable.h>

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

	return NET_HND_FORWARD;
}

net_node_t pnet_get_dev_by_device(struct net_device *dev) {
	net_node_t node = dev->pnet_node;

	if (NULL == node) {
		return pnet_get_module("devs entry");
	}

	return node;
}

net_node_t pnet_get_dev_by_name(char *name) {
	struct net_device *dev;

	netdev_foreach(dev) {
		if (!strcmp(dev->name, name)) {
			return dev->pnet_node;
		}
	}

	return NULL;
}

PNET_NODE_DEF("devs resolver", {
	.rx_hnd = net_dev_rx_hnd,
	.tx_hnd = NULL,
});

static int init(void) {
	net_node_t entry, resolver;
	struct pnet_graph *graph = pnet_graph_create("devs resolver");

	resolver = pnet_get_module("devs resolver");
	pnet_graph_add_src(graph, resolver);
	pnet_graph_start(graph);

	entry = pnet_get_module("pnet entry");
	entry->rx_dfault = resolver;

	return 0;
}
