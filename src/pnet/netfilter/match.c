/**
 * NET packet matchers
 * @date 23.10.11
 * @author Alexander Kalmuk
 */

#include <net/ip.h>
#include <pnet/core.h>
#include <pnet/match.h>
#include <pnet/types.h>
#include <net/if_ether.h>
#include <util/member.h>
#include <string.h>
#include <mem/objalloc.h>
#include <lib/list.h>
#include <assert.h>
#include <net/udp.h>

#define NET_NODES_CNT 0x10

OBJALLOC_DEF(matcher_nodes, struct net_node_matcher, NET_NODES_CNT);

int add_new_rx_rule(match_rule_t new_rule, net_node_matcher_t node) {

	list_add_tail(&new_rule->lnk, &node->match_rx_rules);

	return 0;
}

int hwaddrs_rule_create(net_node_matcher_t node, char *h_src,
		net_node_t next_node) {
	match_rule_t new_rule = pnet_rule_alloc();

	memcpy((void*) new_rule->skbuf->mac.ethh->h_source, (void*) h_src,
			ETH_ALEN);
	new_rule->next_node = next_node;

	add_new_rx_rule(new_rule, node);

	return 0;
}

int match(net_packet_t packet) {
	unsigned char *pack_curr, *rule_curr;
	net_node_matcher_t node;
	match_rule_t curr;
	struct list_head *h;
	size_t n;

	node = (net_node_matcher_t) packet->node;

	list_for_each (h, &node->match_rx_rules) {
		curr = member_cast_out(h, struct match_rule, lnk);
		rule_curr = curr->header;
		pack_curr = (unsigned char*) packet->skbuf->data;

		for (n = MAX_PACK_HEADER_SIZE;
				((*pack_curr == *rule_curr) || (*rule_curr == -1)) && n;
				--n, ++pack_curr, ++rule_curr)
			;

		if (n == 0) {
			packet->node = curr->next_node;
			return 0;
		}
	}

	return NET_HND_DFAULT;
}

static int matcher_free(net_node_t node) {
	net_node_matcher_t m = (net_node_matcher_t) node;

	objfree(&matcher_nodes, m);

	return 0;
}

static struct net_proto hwaddr_matcher_proto = { .tx_hnd = match, .rx_hnd =
		match, .free = matcher_free };

net_node_matcher_t pnet_get_node_matcher(void) {
	net_node_matcher_t matcher = objalloc(&matcher_nodes);

	pnet_node_init(&matcher->node, 0, &hwaddr_matcher_proto);

	INIT_LIST_HEAD(&matcher->match_rx_rules);
	INIT_LIST_HEAD(&matcher->match_tx_rules);

	return matcher;
}
