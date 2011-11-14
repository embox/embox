/**
 * NET rules collection initialization
 * @date 31.10.11
 * @author Alexander Kalmuk
 */

#include <pnet/match.h>
#include <pnet/core.h>
#include <pnet/types.h>
#include <pnet/node.h>
#include <util/array.h>
#include <embox/unit.h>
#include <ctype.h>
#include <framework/cmd/api.h>
#include <net/util.h>
#include <net/if_ether.h>
#include <pnet/repo.h>
#include <util/member.h>
#include <stdio.h>
#include <pnet/graph.h>

#define RULE_ELEMS_COUNT 6

EMBOX_UNIT_INIT(init);

static net_node_t match_node;

static const char *rules[] = {
	#include <pnet/pnet_rules.inc>
};

static int form_rule(match_rule_t rule, const char *rule_elem, int num) {
	if ('*' == rule_elem[0]) {
		return 0;
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
	case 6:
		rule->next_node = pnet_get_module(rule_elem);
		break;
	}

	return 0;
}

static int init(void) {
	const char *rule_elem;
	int cur = 0;
	match_rule_t new_rule;
	net_node_matcher_t match;
	net_node_t match_node;

	match_example();
	new_rule = pnet_rule_alloc();

	array_foreach(rule_elem, rules, ARRAY_SIZE(rules)) {
		if (strncmp("RULES", rule_elem, 5) == 0) {
			match = (net_node_matcher_t) match_node;
		} else {
			cur++;
			form_rule(new_rule, rule_elem, cur);

			if (cur > 0 && cur % RULE_ELEMS_COUNT == 0) {
				pnet_add_new_rx_rule(new_rule, match);
				new_rule = pnet_rule_alloc();
			}
		}
	}

	return 0;
}
