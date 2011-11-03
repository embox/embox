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
#include <stdio.h>
#include <embox/unit.h>
#include <pnet/node.h>

#include <pnet/repo.h>

#define NET_NODES_CNT 0x10

#define PRINT_WAYS

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

static int match(net_packet_t packet) {
	unsigned char *pack_curr, *rule_curr;
	net_node_matcher_t node;
	match_rule_t curr;
	struct list_head *h;
	size_t n;

	node = (net_node_matcher_t) packet->node;

	list_for_each (h, &node->match_rx_rules) {
		curr = member_cast_out(h, struct match_rule, lnk);
		rule_curr = &curr->header[0];
		pack_curr = (unsigned char*) packet->skbuf->data;

		for (n = MAX_PACK_HEADER_SIZE;
				(((*rule_curr == 255) || *pack_curr == *rule_curr)) && n;
				--n, ++pack_curr, ++rule_curr)
			;

		if (n == 0) {
			packet->node = curr->next_node;
#ifdef PRINT_WAYS
		print_pack_way(packet,curr,n);
#endif
			return 0;
		}
	}
#ifdef PRINT_WAYS
		print_pack_way(packet,curr,n);
#endif
	return NET_HND_DFAULT;
}

static int matcher_free(net_node_t node) {
	net_node_matcher_t m = (net_node_matcher_t) node;

	objfree(&matcher_nodes, m);

	return 0;
}

#if 0
static struct pnet_proto matcher_proto = {
	.tx_hnd = match,
	.rx_hnd = match,
	.free = matcher_free
};

net_node_matcher_t pnet_get_node_matcher(void) {
	net_node_matcher_t matcher = objalloc(&matcher_nodes);

	pnet_node_init(&matcher->node, 1, &matcher_proto);
	pnet_node_add_name(&matcher->node,"matcher");

	INIT_LIST_HEAD(&matcher->match_rx_rules);
	INIT_LIST_HEAD(&matcher->match_tx_rules);

	return matcher;
}

#endif
static int matcher_init(struct net_node *node) {
	net_node_matcher_t matcher = (net_node_matcher_t) node;

	INIT_LIST_HEAD(&matcher->match_rx_rules);
	INIT_LIST_HEAD(&matcher->match_tx_rules);

	return 0;
}

PNET_PROTO_DEF("matcher", {
	.name = "matcher",
	.rx_hnd = match,
	.init = matcher_init,
	.free = matcher_free
});
