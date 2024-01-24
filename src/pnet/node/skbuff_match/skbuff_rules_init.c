/**
 * @file
 * @brief Graph for priority NET initialization
 *
 * @date 31.10.11
 * @author Alexander Kalmuk
 */

#include <pnet/node/skbuff_match/netfilter/match_lin.h>
#include <pnet/core/core.h>
#include <pnet/core/types.h>
#include <pnet/core/node.h>
#include <lib/libds/array.h>
#include <embox/unit.h>
#include <ctype.h>
#include <framework/cmd/api.h>
#include <net/util/macaddr.h>
#include <arpa/inet.h>
#include <net/l2/ethernet.h>
#include <pnet/core/repo.h>
#include <util/member.h>
#include <stdio.h>
#include <pnet/core/graph.h>
#include <util/macro.h>

#define RULE_ELEMS_COUNT 6
#define NODES_COUNT 10

EMBOX_UNIT_INIT(init);

//TODO naming nodes as in file
struct __node {
	char *name;
	net_node_t node;
};

static struct __node nodes[NODES_COUNT];

static net_node_t get_node_by_name(char *name);
static char* parse_type(const char *rule);

static const char *rules[] = {
	#include <pnet_rules.inc>
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
		inet_aton(rule_elem,
				(struct in_addr *)&rule->skbuf->nh.iph->saddr);
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
		rule->next_node = get_node_by_name((char*)rule_elem);
		break;
	}

	return 0;
}

static char* parse_type(const char *rule) {
	char *cur;

	for(cur = (char*)rule; *cur != ':'; cur++)
		;

	*cur = '\0';
	cur++;

	return cur;
}

static net_node_t get_node_by_name(char *name) {
	for (size_t i = 0; i < NODES_COUNT; i++) {
		if (strcmp(nodes[i].name, name) == 0) {
			return nodes[i].node;
		}
	}

	return NULL;
}

static int init(void) {
	const char *rule_elem;
	char *tmp;
	int cur = 0;
	int node_cur = 0;
	size_t i = 0;
	match_rule_t new_rule;
	net_node_t src_node;
	net_node_t dst_node;
	net_node_t match_node;
	net_node_matcher_t match;
	struct pnet_graph *graph;

	new_rule = pnet_rule_alloc();

	graph = NULL;
	for(i = 0; i < ARRAY_SIZE(rules); i++) {
		rule_elem = rules[i];

		if(strncmp("TYPE", rule_elem, 4) == 0) {
			tmp = (char*)rule_elem;
			rule_elem = parse_type(rule_elem);
			nodes[node_cur].node = pnet_get_module(tmp + 5);
			nodes[node_cur].name = (char*)rule_elem;
			pnet_graph_add_node(graph, nodes[node_cur++].node);
		} else if(strncmp("GRAPH", rule_elem, 5) == 0) {
			graph = pnet_graph_create("lin_graph");
		} else if (strncmp("RULES", rule_elem, 5) == 0) {
			match_node =  get_node_by_name((char*)rule_elem + 6);
			match = (net_node_matcher_t) match_node;
			cur = 0;
		} else if(strncmp("LINK_SRC", rule_elem, 8) == 0) {
			src_node = get_node_by_name((char*)rule_elem + 9);
			rule_elem = rules[++i];
			dst_node = get_node_by_name((char*)rule_elem + 9);
			pnet_node_link(src_node, dst_node);
		} else {
			cur++;
			form_rule(new_rule, rule_elem, cur);

			if (cur > 0 && cur % RULE_ELEMS_COUNT == 0) {
				pnet_add_new_rx_rule(new_rule, match);
				new_rule = pnet_rule_alloc();
			}
		}
	}

	pnet_graph_start(graph);

	return 0;
}
