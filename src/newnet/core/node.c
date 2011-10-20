/**
 * @file
 * @brief Network node manager
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#include <stddef.h>
#include <net/types.h>
#include <mem/objalloc.h>
#define NET_NODES_CNT 0x10

OBJALLOC_DEF(net_nodes, struct net_node, NET_NODES_CNT);

net_node_t net_node_alloc(net_addr_t addr, net_proto_t proto) {
	net_node_t node = (net_node_t) objalloc(&net_nodes);
	node->node_addr = addr;
	node->proto = proto;

	for (int i = 0; i < CHILD_CNT; i++) {
		node->children[i] = NULL;
	}

	return node;
}

void net_node_free(net_node_t node) {
	objfree(&net_nodes, node);
}
