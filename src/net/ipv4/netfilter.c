/**
 * @file
 * @brief
 *
 * @date 12.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <mem/misc/pool.h>
#include <framework/mod/options.h>
#include <assert.h>
#include <net/netfilter.h>
#include <net/skbuff.h>
#include <net/ip.h>
#include <string.h>
#include <stddef.h>

#include <net/udp.h>
#include <net/tcp.h>

#define MODOPS_NETFILTER_AMOUNT_RULES  OPTION_GET(NUMBER, amount_rules)

/**
 * Storage of nf_rule structure
 */
POOL_DEF(nf_rule_pool, struct nf_rule, MODOPS_NETFILTER_AMOUNT_RULES);

/**
 * Default chains of rules
 */
LIST_DEF(nf_input_rules);
LIST_DEF(nf_forward_rules);
LIST_DEF(nf_output_rules);

/**
 * Default policy for chains
 */
static enum nf_target nf_input_default_target = NF_TARGET_ACCEPT;
static enum nf_target nf_forward_default_target = NF_TARGET_ACCEPT;
static enum nf_target nf_output_default_target = NF_TARGET_ACCEPT;

static void free_rule(struct nf_rule *r) {
	assert(r != NULL);
	list_unlink_link(&r->lnk);
	pool_free(&nf_rule_pool, r);
}

int nf_chain_get_by_name(const char *chain_name) {
	return chain_name == NULL ? NF_CHAIN_UNKNOWN
			: 0 == strcmp(chain_name, "INPUT") ? NF_CHAIN_INPUT
			: 0 == strcmp(chain_name, "FORWARD") ? NF_CHAIN_FORWARD
			: 0 == strcmp(chain_name, "OUTPUT") ? NF_CHAIN_OUTPUT
			: NF_CHAIN_UNKNOWN;
}

const char * nf_chain_to_str(int chain) {
	return chain == NF_CHAIN_INPUT ? "INPUT"
			: chain == NF_CHAIN_FORWARD ? "FORWARD"
			: chain == NF_CHAIN_OUTPUT ? "OUTPUT"
			: NULL;
}

enum nf_target nf_target_get_by_name(const char *target_name) {
	return target_name == NULL ? NF_TARGET_UNKNOWN
			: 0 == strcmp(target_name, "DROP") ? NF_TARGET_DROP
			: 0 == strcmp(target_name, "ACCEPT") ? NF_TARGET_ACCEPT
			: NF_TARGET_UNKNOWN;
}

const char * nf_target_to_str(enum nf_target target) {
	return target == NF_TARGET_DROP ? "DROP"
			: target == NF_TARGET_ACCEPT ? "ACCEPT"
			: NULL;
}

enum nf_proto nf_proto_get_by_name(const char *proto_name) {
	return proto_name == NULL ? NF_PROTO_UNKNOWN
			: 0 == strcmp(proto_name, "all") ? NF_PROTO_ALL
			: 0 == strcmp(proto_name, "icmp") ? NF_PROTO_ICMP
			: 0 == strcmp(proto_name, "tcp") ? NF_PROTO_TCP
			: 0 == strcmp(proto_name, "udp") ? NF_PROTO_UDP
			: NF_PROTO_UNKNOWN;
}

const char * nf_proto_to_str(enum nf_proto proto) {
	return proto == NF_PROTO_ALL ? "all"
			: proto == NF_PROTO_ICMP ? "icmp"
			: proto == NF_PROTO_TCP ? "tcp"
			: proto == NF_PROTO_UDP ? "udp"
			: NULL;
}

struct list * nf_get_chain(int chain) {
	return chain == NF_CHAIN_INPUT ? &nf_input_rules
			: chain == NF_CHAIN_FORWARD ? &nf_forward_rules
			: chain == NF_CHAIN_OUTPUT ? &nf_output_rules
			: NULL;
}

enum nf_target nf_get_chain_target(int chain) {
	return chain == NF_CHAIN_INPUT ? nf_input_default_target
			: chain == NF_CHAIN_FORWARD ? nf_forward_default_target
			: chain == NF_CHAIN_OUTPUT ? nf_output_default_target
			: NF_TARGET_UNKNOWN;
}

int nf_set_chain_target(int chain, enum nf_target target) {
	if (((chain != NF_CHAIN_INPUT) && (chain != NF_CHAIN_FORWARD)
				&& (chain != NF_CHAIN_OUTPUT))
			|| (target == NF_TARGET_UNKNOWN)) {
		return -EINVAL;
	}

	*(chain == NF_CHAIN_INPUT ? &nf_input_default_target
			: chain == NF_CHAIN_FORWARD ? &nf_forward_default_target
			: &nf_output_default_target) = target;

	return 0;
}

struct nf_rule * nf_get_rule_by_num(int chain, size_t r_num) {
	struct list *rules;
	size_t i;
	struct nf_rule *r;

	rules = nf_get_chain(chain);
	if (rules == NULL) {
		return NULL;
	}

	i = 0;
	list_foreach(r, rules, lnk) {
		if (i == r_num) {
			return r;
		}
		++i;
	}

	return NULL;
}

int nf_rule_init(struct nf_rule *r) {
	if (r == NULL) {
		return -EINVAL;
	}

	memset(r, 0, sizeof *r);
	list_link_init(&r->lnk);
	r->not_saddr = r->not_daddr = r->not_sport = r->not_dport = 1;

	return 0;
}

int nf_rule_copy(struct nf_rule *r_dst,
		const struct nf_rule *r_src) {
	struct list_link dst_link;

	if ((r_dst == NULL) || (r_src == NULL)) {
		return -EINVAL;
	}

	memcpy(&dst_link, &r_dst->lnk, sizeof dst_link);
	memcpy(r_dst, r_src, sizeof *r_dst);
	memcpy(&r_dst->lnk, &dst_link, sizeof r_dst->lnk);

	return 0;
}

int nf_add_rule(int chain, const struct nf_rule *r) {
	struct list *rules;
	struct nf_rule *new_r;

	rules = nf_get_chain(chain);
	if (rules == NULL) {
		return -EINVAL;
	}

	if (r == NULL) {
		return -EINVAL;
	}

	new_r = pool_alloc(&nf_rule_pool);
	if (new_r == NULL) {
		return -ENOMEM;
	}

	nf_rule_init(new_r);
	nf_rule_copy(new_r, r);

	list_add_last_link(&new_r->lnk, rules);

	return 0;
}

int nf_set_rule(int chain, const struct nf_rule *r, size_t r_num) {
	struct nf_rule *new_r;

	if (r == NULL) {
		return -EINVAL;
	}

	new_r = nf_get_rule_by_num(chain, r_num);
	if (new_r == NULL) {
		return -ENOENT;
	}

	nf_rule_copy(new_r, r);

	return 0;
}

int nf_del_rule(int chain, size_t r_num) {
	struct nf_rule *r;

	r = nf_get_rule_by_num(chain, r_num);
	if (r == NULL) {
		return -ENOENT;
	}

	free_rule(r);

	return 0;
}

int nf_clear(int chain) {
	struct list *rules;
	struct nf_rule *r;

	rules = nf_get_chain(chain);
	if (rules == NULL) {
		return -EINVAL;
	}

	list_foreach(r, rules, lnk) {
		free_rule(r);
	}

	return 0;
}

int nf_test_rule(int chain, const struct nf_rule *test_r) {
	struct list *rules;
	struct nf_rule *r;

	rules = nf_get_chain(chain);
	if (rules == NULL) {
		return -EINVAL;
	}

	if (test_r == NULL) {
		return -EINVAL;
	}

#define NF_TEST_NOT_FIELD(r1, r2, field) \
	((0 == memcmp(&r1->field, &r2->field, sizeof r1->field))\
		== (r1->not_##field == r2->not_##field))
	list_foreach(r, rules, lnk) {
		if ((r->target != NF_TARGET_UNKNOWN)
				&& NF_TEST_NOT_FIELD(test_r, r, saddr)
				&& NF_TEST_NOT_FIELD(test_r, r, daddr)
				&& (((test_r->proto != NF_PROTO_ALL)
						&& (r->proto != NF_PROTO_ALL)
						&& NF_TEST_NOT_FIELD(test_r, r, proto))
					|| ((test_r->proto == NF_PROTO_ALL) && !test_r->not_proto
						&& (r->proto == NF_PROTO_ALL) && !r->not_proto)
					|| ((test_r->proto != NF_PROTO_ALL)
						&& ((r->proto == NF_PROTO_ALL) && !r->not_proto)))
				&& NF_TEST_NOT_FIELD(test_r, r, sport)
				&& NF_TEST_NOT_FIELD(test_r, r, dport)
				) {
			return test_r->target != r->target;
		}
	}
#undef NF_TEST_NOT_FIELD

	return test_r->target != nf_get_chain_target(chain);
}

int nf_test_skb(int chain, enum nf_target target,
		const struct sk_buff *test_skb) {
	struct nf_rule rule;

	if (test_skb == NULL) {
		return -EINVAL;
	}

	nf_rule_init(&rule);
	rule.target = target;
	rule.saddr.s_addr = test_skb->nh.iph->saddr;
	rule.daddr.s_addr = test_skb->nh.iph->daddr;
	rule.not_saddr = rule.not_daddr = 0;
	switch (test_skb->nh.iph->proto) {
	case IPPROTO_ICMP:
		rule.proto = NF_PROTO_ICMP;
		break;
	case IPPROTO_TCP:
		rule.proto = NF_PROTO_TCP;
		rule.sport = test_skb->h.th->source;
		rule.dport = test_skb->h.th->dest;
		rule.not_sport = rule.not_dport = 0;
		break;
	case IPPROTO_UDP:
		rule.proto = NF_PROTO_UDP;
		rule.sport = test_skb->h.uh->source;
		rule.dport = test_skb->h.uh->dest;
		rule.not_sport = rule.not_dport = 0;
		break;
	}

	return nf_test_rule(chain, &rule);
}
