/**
 * @file
 * @brief
 *
 * @date 12.03.13
 * @author Ilia Vaprol
 */

#ifndef NET_NETFILTER_H_
#define NET_NETFILTER_H_

#include <util/list.h>
#include <netinet/in.h>
#include <net/skbuff.h>

#define NF_CHAIN_INPUT   0
#define NF_CHAIN_FORWARD 1
#define NF_CHAIN_OUTPUT  2

#define NF_POLICY_DROP   0
#define NF_POLICY_ACCEPT 1

struct nf_inet_addr {
	struct in_addr in;
};

struct nf_rule {
	struct list_link lnk;
	int policy;
	int not_addr;
	struct nf_inet_addr addr;
};

extern struct list nf_input_rules;
extern struct list nf_forward_rules;
extern struct list nf_output_rules;

extern int nf_add_rule(int type, const struct nf_rule *r, int overwrite);
extern int nf_del_rule(int type, const struct nf_rule *r);
extern int nf_clear(void);
extern int nf_valid_skb(int type, const struct sk_buff *skb);

#endif /* NET_NETFILTER_H_ */
