/**
 * NET rules collection initialization
 * @date 31.10.11
 * @author Alexander Kalmuk
 */

#include <pnet/match.h>
#include <pnet/core.h>
#include <pnet/match.h>
#include <pnet/types.h>
#include <pnet/node.h>
#include <util/array.h>
#include <embox/unit.h>
#include <ctype.h>
#include <framework/cmd/api.h>
#include <net/util.h>
#include <net/if_ether.h>

#define MAX_RULE_SIZE    100
#define RULE_ELEMS_COUNT 5

EMBOX_UNIT_INIT(init);

static const char *rules[] = {
	#include <pnet/pnet_rules.inc>
};

static match_rule_t form_rule(match_rule_t rule, const char *rule_elem, int num) {
	if ('*' == rule_elem[0]) {
		return rule;
	}

	switch (num) {
	case 1:
		macaddr_scan((unsigned char *) rule_elem,
				rule->skbuf->mac.ethh->h_source);
		break;
	case 2:
		if (strcmp("IP", rule_elem) == 0) {
			pnet_rule_set_pack_type(rule, ETH_P_IP);
		} else if (strcmp("ARP", rule_elem) == 0) {
			pnet_rule_set_pack_type(rule, ETH_P_ARP);
		}
		break;
	case 3:
		ipaddr_scan((unsigned char *) rule_elem,
				(unsigned char*) &rule->skbuf->nh.iph->saddr);
		break;
	case 4:
		if (strcmp("ICMP", rule_elem) == 0) {
			pnet_rule_set_proto(rule, 0x1);
		} else if (strcmp("UDP", rule_elem) == 0) {
			pnet_rule_set_proto(rule, 0x11);
		}
		break;
	case 5:
		pnet_rule_set_src_udp_port(rule, *(uint16_t*) rule_elem);
		break;
	}

	return rule;
}

static int init(void) {
	const char *rule_elem;
	int cur = 0;
	match_rule_t new_rule;
	net_node_matcher_t match;
	//net_node_t info;
	net_node_t match_node;
	net_node_t devs;
	net_node_t tmp;
	net_node_t lin_gate;

	devs = pnet_dev_get_entry();
	lin_gate = pnet_get_node_linux_gate();

	match = pnet_get_node_matcher();
	match_node = (net_node_t) match;

	tmp = pnet_node_get(devs, NET_RX_DFAULT);
	pnet_node_attach(devs, NET_RX_DFAULT, match_node);

	pnet_node_attach(match_node, NET_RX_DFAULT, lin_gate);


	new_rule = pnet_rule_alloc();
	new_rule->next_node = lin_gate;

	array_foreach(rule_elem, rules, ARRAY_SIZE(rules)) {
		cur++;
		new_rule = form_rule(new_rule, rule_elem, cur);

		if (cur > 0 && cur % RULE_ELEMS_COUNT == 0) {
			pnet_add_new_rx_rule(new_rule, match);
			new_rule = pnet_rule_alloc();
		}
	}

	return 0;
}
