/**
 * NET packet matchers
 * @date 23.10.11
 * @author Alexander Kalmuk
 */

#include <pnet/types.h>
#include <net/if_ether.h>
#include <util/member.h>
#include <pnet/prior_path.h>
#include <string.h>
#include <mem/objalloc.h>
#include <lib/list.h>
#include <assert.h>

OBJALLOC_DEF(match_rules, match_rule_t, MAX_RULE_COUNT);

LIST_HEAD(rule);

int add_new_rule(match_rule_t *new_rule) {
	list_add(&new_rule->lnk, &rule);
	return 0;
}

int hwaddrs_rool_create(unsigned char *h_dest, net_node_t *node) {
	match_rule_t new_rule;

	new_rule = (match_rule_t) objalloc(&match_rules);
	strncpy(&new_rule->ether_header.h_dest, h_dest, ETH_ALEN);
	new_rule->node = node;
	list_add(&new_rule->lnk, &rule);
	return 0;
}

net_node_t *match_hwaddrs(net_packet_t packet) {
	struct ethhdr *h_dest;
	match_rule_t *curr;

	h_dest = packet->skbuf->mac.ethh->h_dest;
	list_for_each ((struct list_head *)curr, &rule) {
		if ((curr->ether_header != NULL)
				&& (memcmp(curr->ether_header->h_dest, h_dest, ETH_ALEN))) {
			return curr->next_node;
		}
	}

	return curr->node->dfault;
}
