/**
 * @file
 * @brief Network node manager
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#include <net/types.h>

#define NET_NODES_CNT 0x10

OBJALLOC_DEF(net_nodes, struct net_node, NET_NODES_CNT);

net_node_t net_node_alloc(void) {
	net_node_t node = (net_node_t) objalloc(&net_nodes);
	return node;
}

void net_node_free(net_node_t node) {
	objfree(&net_nodes, node);
}
