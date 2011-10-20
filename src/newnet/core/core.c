/**
 * @file
 * @brief Network core
 *
 * @date 20.10.2011
 * @author Anton Kozlov
 */

#include <errno.h>
#include <net/types.h>
#include <net/core.h>
#include <mem/objalloc.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(net_core_init);

#define NET_PACKS_CNT 0x10

OBJALLOC_DEF(net_packs, struct net_packet, NET_PACKS_CNT);

net_packet_t net_pack_alloc(net_node_t node, enum net_packet_dir dir, void *data, int len) {
	net_packet_t pack = objalloc(&net_packs);

	pack->node = node;
	pack->data = data;
	pack->len  = len;
	pack->dir  = dir;

	return pack;
}

int net_pack_free(net_packet_t pack) {
	objfree(&net_packs, pack);
	return 0;
}

static int __net_core_send(net_packet_t pack) {
	net_node_t node = pack->node;
	int res = 0;

	if (node->proto->tx_hnd != NULL) {
		res = node->proto->tx_hnd(pack);
	}

	if (res == 0) {
		pack->node = pack->node->parent;
		__net_core_send(pack);
	}

	return ENOERR;
}

int net_core_send(net_node_t node, void *data, int len) {
	net_packet_t pack = net_pack_alloc(node, NET_PACKET_TX, data, len);

	return __net_core_send(pack);
}

int __net_core_receive(net_packet_t pack) {
//	net_node_t node = pack->node;

	return 0;
}

int net_core_receive(net_node_t node, void *data, int len) {
	net_packet_t pack = net_pack_alloc(node, NET_PACKET_RX, data, len);

	return __net_core_receive(pack);
}

static int net_core_init(void) {

	return 0;
}
