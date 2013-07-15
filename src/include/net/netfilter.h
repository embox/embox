/**
 * @file
 * @brief
 *
 * @date 12.03.13
 * @author Ilia Vaprol
 */

#ifndef NET_NETFILTER_H_
#define NET_NETFILTER_H_

#include <net/skbuff.h>
#include <netinet/in.h>
#include <util/list.h>

/**
 * Netfilter chains
 */
enum {
	NF_CHAIN_UNKNOWN = 0,
	NF_CHAIN_INPUT,
	NF_CHAIN_FORWARD,
	NF_CHAIN_OUTPUT
};

/**
 * Netfilter targets
 */
enum nf_target {
	NF_TARGET_UNKNOWN = 0,
	NF_TARGET_DROP,
	NF_TARGET_ACCEPT
};

/**
 * Netfilter protocols
 */
enum nf_proto {
	NF_PROTO_ALL = 0,
	NF_PROTO_ICMP,
	NF_PROTO_TCP,
	NF_PROTO_UDP,
	NF_PROTO_UNKNOWN
};

/**
 * Default netfilter limit
 */
#define NF_LIMIT_DEFAULT ((size_t)-1)

/**
 * Netfilter rule structure
 */
struct nf_rule {
	struct list_link lnk;
	enum nf_target target;
	size_t truly;
	size_t limit;
	char hwaddr_dst[ETH_ALEN]; int not_hwaddr_dst;
	char hwaddr_src[ETH_ALEN]; int not_hwaddr_src;
	size_t hwaddr_len;
	struct in_addr saddr; int not_saddr;
	struct in_addr daddr; int not_daddr;
	enum nf_proto proto; int not_proto;
	in_port_t sport; int not_sport;
	in_port_t dport; int not_dport;
};

/**
 * Default chains
 */
extern struct list nf_input_rules;
extern struct list nf_forward_rules;
extern struct list nf_output_rules;

/**
 * Convertion between nf_chain and string
 */
extern int nf_chain_get_by_name(const char *chain_name);
extern const char * nf_chain_to_str(int chain);

/**
 * Convertion between nf_target and string
 */
extern enum nf_target nf_target_get_by_name(const char *target_name);
extern const char * nf_target_to_str(enum nf_target target);

/**
 * Convertion between nf_proto and string
 */
extern enum nf_proto nf_proto_get_by_name(const char *proto_name);
extern const char * nf_proto_to_str(enum nf_proto proto);

/**
 * Netfilter getters/setters
 */
extern struct list * nf_get_chain(int chain);
extern enum nf_target nf_get_chain_target(int chain);
extern int nf_set_chain_target(int chain, enum nf_target target);
extern struct nf_rule * nf_get_rule_by_num(int chain, size_t r_num);

/**
 * Netfilter rule utility
 */
extern int nf_rule_init(struct nf_rule *r);
extern int nf_rule_copy(struct nf_rule *r_dst,
		const struct nf_rule *r_src);

/**
 * Main netfilter API
 */
extern int nf_add_rule(int chain, const struct nf_rule *r);
extern int nf_set_rule(int chain, const struct nf_rule *r,
		size_t r_num);
extern int nf_del_rule(int chain, size_t r_num);
extern int nf_clear(int chain);
extern int nf_test_rule(int chain, const struct nf_rule *test_r);
extern int nf_test_skb(int chain, enum nf_target target,
		const struct sk_buff *test_skb);

/**
 * @brief Perform netfitler test on hw addresses before any parsing has occured.
 * Intended to be called from irq context before skb allocation, etc.
 *
 * @param chain Chain packet to pass
 * @param target Target action of rule
 * @param hwaddr_dst HW address of destination
 * @param hwaddr_src HW address of source
 * @param hwaddr_len HW address length
 *
 * @return
 */
extern int nf_test_raw(int chain, enum nf_target target, const void *hwaddr_dst,
		const void *hwaddr_src, size_t hwaddr_len);
#endif /* NET_NETFILTER_H_ */
