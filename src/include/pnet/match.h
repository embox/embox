/**
 * @date 24.10.11
 * @Alexander Kalmuk
 */
#ifndef NET_MATCH_H_
#define NET_MATCH_H_

#include <pnet/types.h>

#include <string.h>

#define MAX_RULE_COUNT 0x10

#define MATCH_WILDCARD -1

struct match_rule {
	struct ethhdr ether_header;
	in_addr_t dest_ip;
	in_addr_t src_ip;
	uint8_t proto;
	uint16_t dest_port;
	uint16_t src_port;
	net_node_t next_node;
	struct list_head lnk;
};

typedef struct match_rule *match_rule_t;

typedef struct net_node_matcher {
	struct net_node node;
	struct list_head match_rx_rules;
	struct list_head match_tx_rules;
}*net_node_matcher_t;

static inline void pnet_rule_set_next_node(match_rule_t rule, net_node_t node) {
	rule->next_node = node;
}

static inline void pnet_rule_set_eth_src(match_rule_t rule, char *h_src) {
	memcpy((void*) rule->ether_header.h_source, (void*) h_src, ETH_ALEN);
}

//extern net_node_t pnet_create_matcher(net_hnd rx_matcher, net_hnd tx_matcher);
extern match_rule_t pnet_rule_alloc(void);

extern int add_new_rx_rule(match_rule_t new_rule, net_node_matcher_t node);

extern int hwaddrs_rule_create(net_node_matcher_t node, char *h_dest,
		net_node_t next_node);

extern net_node_matcher_t pnet_get_node_matcher(void);

#endif /* NET_MATCH_H_ */
