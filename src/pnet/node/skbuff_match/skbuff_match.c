/**
 * @file
 * @brief NET packet matchers
 *
 * @date 23.10.11
 * @author Alexander Kalmuk
 */


#include <util/member.h>
#include <string.h>
#include <mem/objalloc.h>
#include <lib/list.h>
#include <assert.h>
#include <stdio.h>
#include <embox/unit.h>

#include <net/ip.h>
#include <net/if_ether.h>
#include <net/udp.h>

#include <pnet/node.h>
#include <pnet/pnet_pack.h>

#include <pnet/repo.h>
#include <pnet/netfilter/match_lin.h>
#include <pnet/core.h>
#include <pnet/types.h>


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

int match_lin(struct pnet_pack *pack) {
	unsigned char *pack_curr, *rule_curr;
	net_node_matcher_t node;
	match_rule_t curr;
	struct list_head *h;
	size_t n;

	node = (net_node_matcher_t) pack->node;

	list_for_each (h, &node->match_rx_rules) {
		struct sk_buff *skb = pack->data;
		curr = member_cast_out(h, struct match_rule, lnk);
		rule_curr = curr->skbuf->data;

		pack_curr = (unsigned char *) skb->data;

		for (n = MAX_PACK_HEADER_SIZE;
				(((*rule_curr == 255) || *pack_curr == *rule_curr)) && n;
				--n, ++pack_curr, ++rule_curr)
			;

		if (n == 0) {
			pack->node = curr->next_node;
			pack->priority = curr->priority;
#ifdef PRINT_WAYS
		print_pack_way(pack,curr,n);
#endif
			return MATCH_SUCCESS;
		}
	}
#ifdef PRINT_WAYS
		print_pack_way(pack,curr,n);
#endif
	return (pack->node ? NET_HND_DFAULT : NET_HND_SUPPRESSED);
}

PNET_PROTO_DEF("matcher", {
	.rx_hnd = match_lin,
	.alloc  = matcher_alloc,
	.free   = matcher_free
});
