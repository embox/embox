/**
 * @file
 * @brief Working with pnet example
 *
 * @date 26.10.2011
 * @author Anton Kozlov
 */

#include <pnet/core.h>
#include <pnet/node.h>
#include <pnet/netfilter/match_lin.h>

#include <framework/example/self.h>
EMBOX_EXAMPLE(pnet_general_example);

static int pnet_general_example(int argc, char **argv) {
	char src_mac[6] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01 };

	net_node_t tmp;
	net_node_t info;
	match_rule_t rule;

	net_node_t devs = pnet_dev_get_entry();
	net_node_t lin_gate = pnet_get_node("gate lin");

	net_node_matcher_t match = pnet_get_node_matcher();
	net_node_t match_node = (net_node_t) match;

	tmp = pnet_node_get(devs, NET_RX_DFAULT);
	pnet_node_attach(devs, NET_RX_DFAULT, match_node);
	pnet_node_attach(match_node, NET_RX_DFAULT, tmp);

	info = pnet_get_node_info();
	pnet_node_attach(info, NET_RX_DFAULT, lin_gate);



	rule = pnet_rule_alloc();

	pnet_rule_set_mac_src(rule, src_mac);
	pnet_rule_set_next_node(rule, info);

	add_new_rx_rule(rule, match);

	return 0;
}
