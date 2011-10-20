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
#include <mem/objalloc.h>

EMBOX_UNIT_INIT(net_socket_init);

#define NET_SOCKET_CNT 0x10

//OBJALLOC_DEF(sockets, struct net_socket, NET_SOCKET_CNT);

static int rx_hnd(net_packet_t pack) {
	//printf("%s\n", (char *) data);
	return 0;
}

static int tx_hnd(net_packet_t pack) {
#if 0
	int new_len = pack->len + sizeof(net_id_t);
	void *new_data = malloc(new_len);
	(net_id_t *) new_data = pack->id;
	((char *) new_data) += sizeof(net_id_t);
	//TODO free old data
	pack->data = new_data;
	pack->len = new_len;

#endif
	return 0;
}


static struct net_proto socket_proto;

static int net_socket_init(void) {
	net_proto_init(&socket_proto, 0, rx_hnd, tx_hnd);
	return 0;
}

net_socket_t net_socket_open(net_id_t id, net_node_t parent) {
//	net_socket_t sock = (net_socket_t) objalloc(&sockets);

	net_node_t node = net_node_alloc(id, &socket_proto);

	node_attach(node, id, parent);

	return (net_socket_t) node;
}
