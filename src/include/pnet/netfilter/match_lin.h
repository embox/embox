/**
 * @file
 * @brief
 *
 * @date 24.10.11
 * @author Alexander Kalmuk
 */

#ifndef PNET_MATCH_LIN_H_
#define PNET_MATCH_LIN_H_

#include <pnet/types.h>
#include <net/l3/ipv4/ip.h>
#include <net/l4/udp.h>
#include <string.h>
#include <net/skbuff.h>
#include <pnet/core.h>
#include <arpa/inet.h>
#include <pnet/netfilter/match.h>

#define ETH_P_IP_REV	0x8

#define MAX_RULE_COUNT 0x10

#define MATCH_WILDCARD -1

#define MAX_PACK_HEADER_SIZE	0x30

/* TODO make set of rules as a hash table */
struct match_rule {
	struct sk_buff *skbuf;
	net_node_t next_node;
	struct list_head lnk;
	uint32_t priority;
};

typedef struct match_rule *match_rule_t;

typedef struct net_node_matcher {
	struct net_node node;
	struct list_head match_rx_rules;
	struct list_head match_tx_rules;
}*net_node_matcher_t;


/* XXX */
extern net_packet_t get_packet(struct sk_buff *skbuff);

static inline void pnet_rule_set_next_node(match_rule_t rule, net_node_t node) {
	rule->next_node = node;
}

static inline void pnet_rule_set_src_mac(match_rule_t rule, unsigned char *h_src) {
	memcpy((void*) rule->skbuf->mac.ethh->h_source, (void*) h_src, ETH_ALEN);
}

static inline void pnet_rule_set_pack_type(match_rule_t rule, uint16_t type) {
	uint16_t t = ntohs(type);
	memcpy((void*) &rule->skbuf->mac.ethh->h_proto, (void*) &t, sizeof(unsigned short));
	/* TODO */
	switch (type) {
	case ETH_P_IP:
			/* svv: It's correct unless we have options in IP packet */
		/* rule->skbuf->h.raw = rule->skbuf->nh.raw + IP_HEADER_SIZE(rule->skbuf->nh.iph);*/
		break;
	case ETH_P_LOOP:
		break;
	case ETH_P_ARP:
		break;
	case ETH_P_ALL:
		rule->skbuf->mac.ethh->h_proto = -1;
		break;
	}
}

static inline void pnet_rule_set_src_ip(match_rule_t rule, net_addr_t ip_src) {
	pnet_rule_set_pack_type(rule, ETH_P_IP);
	rule->skbuf->nh.iph->saddr = ip_src;
}

static inline void pnet_rule_set_src_udp_port(match_rule_t rule,
		uint16_t src_port) {
	/* svv: Won't work if packet is fragmented. + Check bytes order in port */
	rule->skbuf->h.uh->source = src_port;
}

static inline void pnet_rule_set_proto(match_rule_t rule, unsigned char proto) {
	rule->skbuf->nh.iph->proto = proto;
}

//extern net_node_t pnet_create_matcher(net_hnd rx_matcher, net_hnd tx_matcher);
extern match_rule_t pnet_rule_alloc(void);
extern void pnet_rule_free(match_rule_t rule);

static inline void pnet_add_new_rx_rule(match_rule_t new_rule, net_node_matcher_t node) {
	list_add_tail(&new_rule->lnk, &node->match_rx_rules);
}

static inline void pnet_remove_rx_rule(match_rule_t new_rule) {
	list_del(&new_rule->lnk);
}

extern net_node_matcher_t pnet_get_node_matcher(void);

extern int match_lin(struct pnet_pack *pack);

#define MATCH_SUCCESS 0

#endif /* PNET_MATCH_LIN_H_ */
