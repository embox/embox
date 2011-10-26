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
#define NET_NODES_CNT 0x10

OBJALLOC_DEF(net_nodes, struct net_node, NET_NODES_CNT);

net_node_t pnet_node_alloc(net_addr_t addr, net_proto_t proto) {
	net_node_t node = (net_node_t) objalloc(&net_nodes);
	node->node_addr = addr;
	node->proto = proto;

	node->rx_dfault = node->tx_dfault = NULL;
	return node;
}

void pnet_node_free(net_node_t node) {
	objfree(&net_nodes, node);
}
