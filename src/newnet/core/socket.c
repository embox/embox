/**
 * @file
 * @brief Network socket
 *
 * @date 19.10.2011
 * @author Anton Kozlov
 */

#include <posix/stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/types.h>
#include <net/core.h>
#include <embox/unit.h>
#include <mem/objalloc.h>

EMBOX_UNIT_INIT(net_socket_init);

#define NET_SOCKET_CNT 0x10

OBJALLOC_DEF(sockets, struct net_socket, NET_SOCKET_CNT);

static int rx_hnd(net_packet_t pack) {
	net_id_t id = *((net_id_t *) pack->data);
	pack->data = (void *) (((char *) (pack->data)) + sizeof(net_id_t));

	printf("%d: %s\n", id, (char *) pack->data);
	return -2;
}

static int tx_hnd(net_packet_t pack) {
	int new_len = pack->len + sizeof(net_id_t);
	char *new_data = (char *) malloc(new_len);

	net_id_t *data_header = (net_id_t *) new_data;

	*data_header = pack->node->id;

	memcpy(new_data + sizeof(net_id_t), pack->data, pack->len);

	//TODO free old data, like below
	//free(pack->orig_data);

	pack->data = new_data;
	pack->orig_data = new_data;
	pack->len = new_len;

	return 0;
}


static struct net_proto socket_proto;

static int net_socket_init(void) {
	net_proto_init(&socket_proto, 0, rx_hnd, tx_hnd);
	return 0;
}

net_socket_t net_socket_open(net_id_t id, net_node_t parent) {
	net_socket_t sock = (net_socket_t) objalloc(&sockets);

	node_attach(&sock->node, id, parent);

	sock->node.proto = &socket_proto;

	return sock;
}
