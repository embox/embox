/**
 * @file
 * @brief Network core
 *
 * @date 20.10.2011
 * @author Anton Kozlov
 */

#include <pnet/core.h>
#include <pnet/node.h>
#include <mem/objalloc.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(net_core_init);

#define NET_PACKS_CNT 0x10

OBJALLOC_DEF(net_packs, struct net_packet, NET_PACKS_CNT);

net_packet_t pnet_pack_alloc(net_node_t node, enum net_packet_dir dir, void *data, int len) {
	net_packet_t pack = objalloc(&net_packs);

	pack->node = node;
	pack->data = data;
	pack->len  = len;
	pack->dir  = dir;

	return pack;
}

int pnet_pack_free(net_packet_t pack) {
	objfree(&net_packs, pack);
	return 0;
}

int pnet_core_send(net_node_t node, void *data, int len) {
	net_packet_t pack = pnet_pack_alloc(node, NET_PACKET_TX, data, len);

	return pnet_process(pack);
}

int pnet_core_receive(net_node_t node, void *data, int len) {
	net_packet_t pack = pnet_pack_alloc(node, NET_PACKET_RX, data, len);

	return pnet_process(pack);
}


int pnet_node_attach(net_node_t node, net_id_t id, net_node_t parent) {
	if (parent != NULL) {
		if (id > 0) {
			parent->children[id] = node;
		} else if (id == NET_RX_DFAULT) {
			parent->dfault = node;
		}
	}

	node->prior = 0;
	node->parent = parent;

	return 0;
}

net_node_t pnet_proto_attach(net_proto_t proto, net_addr_t addr, net_node_t parent) {
      	net_node_t node = pnet_node_alloc(addr, proto);

	pnet_node_attach(node, proto->proto_id, parent);

	return node;
}

static int net_core_init(void) {
	net_node_t devs = pnet_dev_get_entry();
	net_node_t info = pnet_get_node_info();
	net_node_t lin_gate = pnet_get_node_linux_gate();

	pnet_node_attach(info, NET_RX_DFAULT, devs);

	pnet_node_attach(lin_gate, NET_RX_DFAULT, info);

	return 0;
}
