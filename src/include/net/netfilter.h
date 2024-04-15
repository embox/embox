/**
 * @file
 * @brief
 *
 * @date 12.03.13
 * @author Ilia Vaprol
 */

#ifndef NET_NETFILTER_H_
#define NET_NETFILTER_H_

#include <net/l2/ethernet.h>
#include <net/skbuff.h>
#include <netinet/in.h>
#include <lib/libds/dlist.h>

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
 * Netfilter test callback
 */
struct nf_rule;

/**
 * @brief Netfilter dynamic matching field
 *
 * @param r Rule to match against
 * @param data Optional data stored in rule
 *
 * @return 0 if rule not matches
 * @return not zero if rule matches
 */
typedef int (*nf_test_hnd)(const struct nf_rule *r,
		void *data);

/**
 * Netfilter rule structure
 */
#define NF_DECL_NOT_FIELD(name, type) \
	typeof(type) name; char set_##name; char not_##name;

struct nf_rule {
	struct dlist_head lnk;
	enum nf_target target;
	NF_DECL_NOT_FIELD(hwaddr_src, char [ETH_ALEN]);
	NF_DECL_NOT_FIELD(hwaddr_dst, char [ETH_ALEN]);
	NF_DECL_NOT_FIELD(saddr, struct in_addr);
	NF_DECL_NOT_FIELD(daddr, struct in_addr);
	NF_DECL_NOT_FIELD(proto, enum nf_proto);
	NF_DECL_NOT_FIELD(sport, in_port_t);
	NF_DECL_NOT_FIELD(dport, in_port_t);
	nf_test_hnd test_hnd;
	void *test_hnd_data;
};

/**
 * Default chains
 */
extern struct dlist_head nf_input_rules;
extern struct dlist_head nf_forward_rules;
extern struct dlist_head nf_output_rules;

/**
 * Convertion between nf_chain and string
 */
extern int nf_chain_get_by_name(const char *chain_name);
extern const char *nf_chain_to_str(int chain);

/**
 * Convertion between nf_target and string
 */
extern enum nf_target nf_target_get_by_name(const char *target_name);
extern const char *nf_target_to_str(enum nf_target target);

/**
 * Convertion between nf_proto and string
 */
extern enum nf_proto nf_proto_get_by_name(const char *proto_name);
extern const char *nf_proto_to_str(enum nf_proto proto);

/**
 * Netfilter getters/setters
 */
extern struct dlist_head *nf_get_chain(int chain);
extern enum nf_target nf_get_chain_target(int chain);
extern int nf_set_chain_target(int chain, enum nf_target target);
extern struct nf_rule *nf_get_rule_by_num(int chain, size_t r_num);

/**
 * Netfilter rule utility
 */
extern int nf_rule_init(struct nf_rule *r);
extern int nf_rule_copy(struct nf_rule *r_dst,
		const struct nf_rule *r_src);

/**
 * Main netfilter API
 */
#define NF_SET_NOT_FIELD(r, field, not, value) \
	do {                                       \
		(r)->field = value;                    \
		(r)->set_##field = 1;                  \
		(r)->not_##field = not;                \
	} while (0)
#define NF_SET_NOT_FIELD_PTR(r, field, not,    \
		val_ptr, val_len)                      \
	do {                                       \
		assert(val_len <= sizeof (r)->field);  \
		memcpy(&(r)->field, val_ptr, val_len); \
		(r)->set_##field = 1;                  \
		(r)->not_##field = not;                \
	} while (0)

extern int nf_add_rule(int chain, const struct nf_rule *r);
extern int nf_insert_rule(int chain, const struct nf_rule *r, size_t r_num);
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
