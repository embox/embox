/**
 * @file
 * @brief Network node manager
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#include <stddef.h>
#include <pnet/types.h>
#include <mem/objalloc.h>
#include <assert.h>
#include <string.h>
#include <util/list.h>

#include <pnet/core.h>

OBJALLOC_DEF(net_nodes, struct net_node, CONFIG_PNET_NODES_QUANTITY);

net_node_t pnet_node_init(net_node_t node, pnet_proto_t proto) {
	assert(node);
	node->proto = proto;

	INIT_LIST_HEAD(&node->gr_link);

	node->rx_dfault = node->tx_dfault = NULL;
	node->graph = NULL;

	return node;
}

net_node_t pnet_node_alloc(net_addr_t addr, pnet_proto_t proto) {
	net_node_t node;
	if (NULL == proto || NULL == proto->actions.alloc) {
		node = (net_node_t) objalloc(&net_nodes);
	} else {
		node = proto->actions.alloc();
	}

	pnet_node_init(node, proto);

	return node;

}

int pnet_node_free(net_node_t node) {
	assert(node);
	if ((NULL != node->proto) && (NULL != node->proto->actions.free)) {
		if (node->proto->actions.free(node) == 0) {
			return 0;
		}
	}
	objfree(&net_nodes, node);
	return 0;
}

int pnet_node_attach(net_node_t node, net_id_t id, net_node_t other) {
	if (node == NULL) {
		return -1;
	}

	switch (id) {
		case NET_RX_DFAULT:
			node->rx_dfault = other;
			break;
		case NET_TX_DFAULT:
			node->tx_dfault = other;
			break;
	}

	return 0;
}

net_node_t pnet_node_get(net_node_t node, net_id_t id) {
	switch (id) {
		case NET_RX_DFAULT:
			return node->rx_dfault;
		case NET_TX_DFAULT:
			return node->tx_dfault;
	}

	return NULL;
}
