/**
 * @date 24.10.11
 * @Alexander Kalmuk
 */

#ifndef NET_MATCH_H_
#define NET_MATCH_H_

#include <pnet/types.h>
#include <net/ip.h>
#include <net/udp.h>
#include <string.h>

#define MAX_RULE_COUNT 0x10

#define MATCH_WILDCARD -1

#define MAX_PACK_HEADER_SIZE	0x100

struct match_rule {
	unsigned char header[MAX_PACK_HEADER_SIZE];
	struct sk_buff *skbuf;
	net_node_t next_node;
	struct list_head lnk;
};

typedef struct match_rule *match_rule_t;

typedef struct net_node_matcher {
	struct net_node node;
	struct list_head match_rx_rules;
	struct list_head match_tx_rules;
}*net_node_matcher_t;

extern int match(net_packet_t packet);

static inline void pnet_rule_set_next_node(match_rule_t rule, net_node_t node) {
	rule->next_node = node;
}

static inline void pnet_rule_set_mac_src(match_rule_t rule, char *h_src) {
	memcpy((void*) rule->skbuf->mac.ethh->h_source, (void*) h_src, ETH_ALEN);
}

static inline void pnet_rule_set_ip_src(match_rule_t rule, net_addr_t ip_src) {
	rule->skbuf->nh.iph->saddr = ip_src;
}

static inline void pnet_rule_set_udp_port_src(match_rule_t rule,
		uint16_t src_port) {
	rule->skbuf->h.uh->source = src_port;
}

static inline void pnet_rule_set_pack_type_ip(match_rule_t rule) {
	rule->skbuf->mac.ethh = (struct ethhdr*) rule->header
				+ sizeof(rule->skbuf->h) + sizeof(rule->skbuf->nh);
	rule->skbuf->mac.ethh->h_proto = 0x0800;
}

//extern net_node_t pnet_create_matcher(net_hnd rx_matcher, net_hnd tx_matcher);
extern match_rule_t pnet_rule_alloc(void);

extern int add_new_rx_rule(match_rule_t new_rule, net_node_matcher_t node);

extern int hwaddrs_rule_create(net_node_matcher_t node, char *h_dest,
		net_node_t next_node);

extern net_node_matcher_t pnet_get_node_matcher(void);

#endif /* NET_MATCH_H_ */
