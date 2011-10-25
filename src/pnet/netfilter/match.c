/**
 * NET packet matchers
 * @date 23.10.11
 * @author Alexander Kalmuk
 */
#include <pnet/core.h>
#include <pnet/match.h>
#include <pnet/types.h>
#include <net/if_ether.h>
#include <util/member.h>
#include <string.h>
#include <mem/objalloc.h>
#include <lib/list.h>
#include <assert.h>

OBJALLOC_DEF(match_rules, match_rule_t, MAX_RULE_COUNT);

net_node_t create_matcher(net_hnd rx_matcher, net_hnd tx_matcher) {
	net_proto_t proto = NULL;

	proto->rx_hnd = rx_matcher;
	proto->tx_hnd = tx_matcher;
	return (net_node_t) pnet_node_alloc((net_addr_t)0, proto);
}

int add_new_rx_rule(match_rule_t *new_rule, net_node_t node) {
	list_add(&new_rule->lnk, &node->match_rx_rules);
	return 0;
}

int hwaddrs_rule_create(char *h_dest, net_node_t node) {
	match_rule_t *new_rule;

	new_rule = (match_rule_t*) objalloc(&match_rules);
	memcpy((void*) new_rule->ether_header.h_dest, (void*) h_dest, ETH_ALEN);
	list_add(&node->match_rx_rules, &new_rule->lnk);
	return 0;
}

int match_hwaddrs(net_packet_t packet) {
	unsigned char *h_dest;
	match_rule_t *curr;
	net_node_t node;
	struct list_head *h;

	node = packet->node;
	h_dest = packet->skbuf->mac.ethh->h_dest;
	h = &curr->lnk;
	list_for_each (h, &node->match_rx_rules) {
		curr = member_cast_out(h, match_rule_t, lnk);
		assert(&curr->ether_header);
		if (memcmp((void*)&curr->ether_header.h_dest, (void*)h_dest, ETH_ALEN)) {
			return curr->next_node;
		}
	}

	return 0;
}
