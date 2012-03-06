/**
 * @file
 * @brief NET packet matchers
 *
 * @date 23.10.11
 * @author Alexander Kalmuk
 */

#include <pnet/match.h>
#include <net/ip.h>
#include <pnet/core.h>
#include <pnet/types.h>
#include <net/if_ether.h>
#include <util/member.h>
#include <string.h>
#include <mem/objalloc.h>
#include <lib/list.h>
#include <assert.h>
#include <net/udp.h>
#include <stdio.h>
#include <embox/unit.h>
#include <pnet/node.h>

#include <pnet/repo.h>

#define NET_NODES_CNT 0x10

OBJALLOC_DEF(matcher_nodes, struct net_node_matcher, NET_NODES_CNT);

#ifdef PRINT_WAYS
static void print_pack_way(net_packet_t pack, match_rule_t rule , int n) {
	net_node_t node;

	printf("%s->", "matcher");

	if (n == 0) {
		node = rule->next_node;
		printf("%s->", node->proto->name);
	} else {
		node = pack->node->rx_dfault;
		printf("%s->", node->proto->name);
	}

	while (node->rx_dfault != NULL) {
		node = node->rx_dfault;
		printf("%s->", node->proto->name);
	}

	printf("%s\n", ";");
}
#endif

int match(struct pnet_pack *pack) {
	unsigned char *pack_curr, *rule_curr;
	net_node_matcher_t node;
	match_rule_t curr;
	struct list_head *h;
	size_t n;

	node = (net_node_matcher_t) pack->node;

	list_for_each (h, &node->match_rx_rules) {
		curr = member_cast_out(h, struct match_rule, lnk);
		rule_curr = &curr->header[0];
		pack_curr = (unsigned char*) pack->skb->data;

		for (n = MAX_PACK_HEADER_SIZE;
				(((*rule_curr == 255) || *pack_curr == *rule_curr)) && n;
				--n, ++pack_curr, ++rule_curr)
			;

		if (n == 0) {
			pack->node = curr->next_node;
#ifdef PRINT_WAYS
		print_pack_way(pack,curr,n);
#endif
			return MATCH_SUCCESS;
		}
	}
#ifdef PRINT_WAYS
		print_pack_way(pack,curr,n);
#endif
	return NET_HND_DFAULT;
}

static int matcher_free(net_node_t node) {
	net_node_matcher_t m = (net_node_matcher_t) node;

	objfree(&matcher_nodes, m);

	return 0;
}

net_node_t matcher_alloc(void) {
	net_node_matcher_t matcher = objalloc(&matcher_nodes);

	INIT_LIST_HEAD(&matcher->match_rx_rules);
	INIT_LIST_HEAD(&matcher->match_tx_rules);

	return (net_node_t) matcher;
}

PNET_PROTO_DEF("matcher", {
	.rx_hnd = match,
	.alloc  = matcher_alloc,
	.free   = matcher_free
});
