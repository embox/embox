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

#define NET_NODES_CNT 0x10

OBJALLOC_DEF(matcher_nodes, struct net_node_matcher, NET_NODES_CNT);

int add_new_rx_rule(match_rule_t new_rule, net_node_matcher_t node) {

	list_add_tail(&new_rule->lnk, &node->match_rx_rules);

	return 0;
}

int hwaddrs_rule_create(net_node_matcher_t node, char *h_src,
		net_node_t next_node) {
	match_rule_t new_rule = pnet_rule_alloc();

	memcpy((void*) new_rule->ether_header.h_source, (void*) h_src, ETH_ALEN);
	new_rule->next_node = next_node;

	add_new_rx_rule(new_rule, node);

	return 0;
}

static int match_hwaddrs(net_packet_t packet, match_rule_t rule) {
	unsigned char *h_src, *t;

	assert(rule != NULL);

	h_src = packet->skbuf->mac.ethh->h_source;
	t = rule->ether_header.h_source;
	if ((memcmp(t, h_src, ETH_ALEN) == 0)
			|| !(t[0] | t[1] | t[2] | t[3] | t[4] | t[5])) {
		return 0;
	}

	return NET_HND_DFAULT;
}

static int match_ip(net_packet_t packet, match_rule_t rule) {
	in_addr_t ip;

	ip = packet->skbuf->nh.iph->saddr;
	assert(rule != NULL);

	for (int i = 0; i < 4; i++) {
		if (rule->src_ip & 255 << i) {
			rule->src_ip |= 255 << i;
			ip |= 255 << i;
		}
	}
	if ((rule->src_ip ^ ip) == 0) {
		return 0;
	}

	return NET_HND_DFAULT;
}

int match(net_packet_t packet) {
	match_rule_t curr;
	net_node_matcher_t node;
	struct list_head *h;

	node = (net_node_matcher_t) packet->node;

	list_for_each (h, &node->match_rx_rules) {
		curr = member_cast_out(h, struct match_rule, lnk);
		if ((0 == match_hwaddrs(packet, curr))
				&& (0 == match_ip(packet, curr))) {
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

static struct net_proto hwaddr_matcher_proto = {
		.tx_hnd = match,
		.rx_hnd = match,
		.free = matcher_free
};

net_node_matcher_t pnet_get_node_matcher(void) {
	net_node_matcher_t matcher = objalloc(&matcher_nodes);

	pnet_node_init(&matcher->node, 0, &hwaddr_matcher_proto);

	INIT_LIST_HEAD(&matcher->match_rx_rules);
	INIT_LIST_HEAD(&matcher->match_tx_rules);

	return matcher;
}

