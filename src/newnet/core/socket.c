/**
 * @file
 * @brief Network socket
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

//#include <stdio.h>
#include <net/types.h>
#include <net/core.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(net_socket_init);

static int rx_hnd(net_node_t this, void *data) {
	//printf("%s\n", (char *) data);
	return 0;
}

net_proto_t socket_proto;

static int net_socket_init(void) {
	net_proto_init(socket_proto, 0, rx_hnd, NULL);
	return 0;
}

net_socket_t net_socket_open(net_id_t id, net_node_t parent) {
	net_node_t node = net_node_alloc(id, socket_proto);
	parent->children[id] = node;
	return (net_socket_t) node;
}
