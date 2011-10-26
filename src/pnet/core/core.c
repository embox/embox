/**
 * @file
 * @brief Network core
 *
 * @date 20.10.2011
 * @author Anton Kozlov
 */

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/match.h>

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

int pnet_node_attach(net_node_t node, net_id_t id, net_node_t other) {
	if (node == NULL) {
		return -1;
	}

	switch (id) {
	case NET_RX_DFAULT:
		node->rx_dfault = other;
		break;
	case NET_TX_DFAULT:
		node->tx_dfault = other;
	}

	return 0;
}

net_node_t pnet_node_get(net_node_t node, net_id_t id) {
	switch (id) {
	case NET_RX_DFAULT:
		return node->rx_dfault;
	case NET_TX_DFAULT:
		return node->tx_dfault;
	}
	return NULL;
}

#if 0
net_node_t pnet_proto_attach(net_proto_t proto, net_addr_t addr, net_node_t parent) {
      	net_node_t node = pnet_node_alloc(addr, proto);

	pnet_node_attach(node, proto->proto_id, parent);

	return node;
}
#endif
static int net_core_init(void) {
	char src_mac[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01 };

	net_node_t devs = pnet_dev_get_entry();
	net_node_t info = pnet_get_node_info();
	net_node_t lin_gate = pnet_get_node_linux_gate();

	net_node_matcher_t match = pnet_get_node_matcher();
	net_node_t match_node = (net_node_t) match;

	match_rule_t rule;

	pnet_node_attach(devs, NET_RX_DFAULT, lin_gate);

	/* */

	pnet_node_attach(devs, NET_RX_DFAULT, match_node);

	pnet_node_attach(match_node, NET_RX_DFAULT, lin_gate);


	/* */

	rule = pnet_rule_alloc();

	pnet_rule_set_eth_src(rule, src_mac);
	pnet_rule_set_next_node(rule, info);

	add_new_rx_rule(rule, match);

	pnet_node_attach(info, NET_RX_DFAULT, lin_gate);

	return 0;
}
