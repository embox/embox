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

#include <util/list.h>

OBJALLOC_DEF(net_nodes, struct net_node, CONFIG_PNET_NODES_QUANTITY);

net_node_t pnet_node_init(net_node_t node, pnet_proto_t proto) {
	node->proto = proto;

	list_link_init(&node->gr_link);

	node->rx_dfault = node->tx_dfault = NULL;
	return node;
}

net_node_t pnet_node_alloc(net_addr_t addr, pnet_proto_t proto) {
	net_node_t node = (net_node_t) objalloc(&net_nodes);
	pnet_node_init(node, proto);
	if (node->proto->init != NULL) {
		if (node->proto->init(node) != 0) {
			objfree(&net_nodes, node);
			return NULL;
		}
	}
	return node;

}

int pnet_node_free(net_node_t node) {
	if (node->proto->free != NULL) {
		if (node->proto->free(node) == 0) {
			return 0;
		}
	}
	objfree(&net_nodes, node);
	return 0;
}
