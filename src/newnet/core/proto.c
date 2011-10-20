/**
 * @file
 * @brief Network protocol manager
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#include <stddef.h>
#include <net/types.h>
#include <net/core.h>

#define NET_PROTO_CNT 0x10

int net_proto_init(net_proto_t proto, net_id_t id, net_hnd rx, net_hnd tx) {
	proto->proto_id = id;
	proto->rx_hnd = rx;
	proto->tx_hnd = tx;
	return 0;
}

int node_attach(net_node_t node, net_id_t id, net_node_t parent) {
	if (id != -1 && parent != NULL) {
		parent->children[id] = node;
	}

	node->parent = parent;
	return 0;
}

net_node_t proto_attach(net_proto_t proto, net_addr_t addr, net_node_t parent) {
      	net_node_t node = net_node_alloc(addr, proto);

	node_attach(node, proto->proto_id, parent);

	return node;
}
