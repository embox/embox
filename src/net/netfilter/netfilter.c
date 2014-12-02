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
#include <net/l3/ipv4/ip.h>
#include <string.h>
#include <stddef.h>

#include <net/l4/udp.h>
#include <net/l4/tcp.h>

#define MODOPS_NETFILTER_AMOUNT_RULES  OPTION_GET(NUMBER, amount_rules)

/**
 * Storage of nf_rule structure
 */
POOL_DEF(nf_rule_pool, struct nf_rule, MODOPS_NETFILTER_AMOUNT_RULES);

/**
 * Default chains of rules
 */
DLIST_DEFINE(nf_input_rules);
DLIST_DEFINE(nf_forward_rules);
DLIST_DEFINE(nf_output_rules);

/**
 * Default policy for chains
 */
static enum nf_target nf_input_default_target = NF_TARGET_ACCEPT;
static enum nf_target nf_forward_default_target = NF_TARGET_ACCEPT;
static enum nf_target nf_output_default_target = NF_TARGET_ACCEPT;

static void free_rule(struct nf_rule *r) {
	assert(r != NULL);
	dlist_del_init(&r->lnk);
	pool_free(&nf_rule_pool, r);
}

int nf_chain_get_by_name(const char *chain_name) {
	if (chain_name == NULL) {
		return NF_CHAIN_UNKNOWN;
	}
	else if (0 == strcmp(chain_name, "INPUT")) {
		return NF_CHAIN_INPUT;
	}
	else if (0 == strcmp(chain_name, "FORWARD")) {
		return NF_CHAIN_FORWARD;
	}
	else if (0 == strcmp(chain_name, "OUTPUT")) {
		return NF_CHAIN_OUTPUT;
	}
	else {
		return NF_CHAIN_UNKNOWN;
	}
}

const char *nf_chain_to_str(int chain) {
	switch (chain) {
	default: return NULL;
	case NF_CHAIN_INPUT: return "INPUT";
	case NF_CHAIN_FORWARD: return "FORWARD";
	case NF_CHAIN_OUTPUT: return "OUTPUT";
	}
}

enum nf_target nf_target_get_by_name(const char *target_name) {
	if (target_name == NULL) {
		return NF_TARGET_UNKNOWN;
	}
	else if (0 == strcmp(target_name, "DROP")) {
		return NF_TARGET_DROP;
	}
	else if (0 == strcmp(target_name, "ACCEPT")) {
		return NF_TARGET_ACCEPT;
	}
	else {
		return NF_TARGET_UNKNOWN;
	}
}

const char *nf_target_to_str(enum nf_target target) {
	switch (target) {
	default: return NULL;
	case NF_TARGET_DROP: return "DROP";
	case NF_TARGET_ACCEPT: return "ACCEPT";
	}
}

enum nf_proto nf_proto_get_by_name(const char *proto_name) {
	if (proto_name == NULL) {
		return NF_PROTO_UNKNOWN;
	}
	else if (0 == strcmp(proto_name, "all")) {
		return NF_PROTO_ALL;
	}
	else if (0 == strcmp(proto_name, "icmp")) {
		return NF_PROTO_ICMP;
	}
	else if (0 == strcmp(proto_name, "tcp")) {
		return NF_PROTO_TCP;
	}
	else if (0 == strcmp(proto_name, "udp")) {
		return NF_PROTO_UDP;
	}
	else {
		return NF_PROTO_UNKNOWN;
	}
}

const char *nf_proto_to_str(enum nf_proto proto) {
	switch (proto) {
	default: return NULL;
	case NF_PROTO_ALL: return "all";
	case NF_PROTO_ICMP: return "icmp";
	case NF_PROTO_TCP: return "tcp";
	case NF_PROTO_UDP: return "udp";
	}
}

struct dlist_head *nf_get_chain(int chain) {
	switch (chain) {
	default: return NULL;
	case NF_CHAIN_INPUT: return &nf_input_rules;
	case NF_CHAIN_FORWARD: return &nf_forward_rules;
	case NF_CHAIN_OUTPUT: return &nf_output_rules;
	}
}

enum nf_target nf_get_chain_target(int chain) {
	switch (chain) {
	default: return NF_TARGET_UNKNOWN;
	case NF_CHAIN_INPUT: return nf_input_default_target;
	case NF_CHAIN_FORWARD: return nf_forward_default_target;
	case NF_CHAIN_OUTPUT: return nf_output_default_target;
	}
}

int nf_set_chain_target(int chain, enum nf_target target) {
	if (target == NF_TARGET_UNKNOWN) {
		return -EINVAL;
	}

	switch (chain) {
	default: return -EINVAL;
	case NF_CHAIN_INPUT:
		nf_input_default_target = target;
		break;
	case NF_CHAIN_FORWARD:
		nf_forward_default_target = target;
		break;
	case NF_CHAIN_OUTPUT:
		nf_output_default_target = target;
		break;
	}

	return 0;
}

struct nf_rule *nf_get_rule_by_num(int chain, size_t r_num) {
	struct dlist_head *rules;
	size_t i;
	struct nf_rule *r;

	rules = nf_get_chain(chain);
	if (rules == NULL) {
		return NULL;
	}

	i = 0;
	dlist_foreach_entry(r, rules, lnk) {
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
	dlist_head_init(&r->lnk);

	return 0;
}

int nf_rule_copy(struct nf_rule *r_dst,
		const struct nf_rule *r_src) {
	struct dlist_head dst_link;

	if ((r_dst == NULL) || (r_src == NULL)) {
		return -EINVAL;
	}

	memcpy(&dst_link, &r_dst->lnk, sizeof dst_link);
	memcpy(r_dst, r_src, sizeof *r_dst);
	memcpy(&r_dst->lnk, &dst_link, sizeof r_dst->lnk);

	return 0;
}

static int nf_chain_rule_prepare(int chain, const struct nf_rule *r,
		struct dlist_head **rules_p, struct nf_rule **new_r_p) {
	struct dlist_head *rules;
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

	*rules_p = rules;
	*new_r_p = new_r;

	return 0;
}


int nf_add_rule(int chain, const struct nf_rule *r) {
	struct dlist_head *rules;
	struct nf_rule *new_r;
	int res;

	res = nf_chain_rule_prepare(chain, r, &rules, &new_r);
	if (res != 0) {
		return res;
	}

	dlist_add_prev(&new_r->lnk, rules);

	return 0;
}

int nf_insert_rule(int chain, const struct nf_rule *r, size_t num) {
	struct dlist_head *rules;
	struct nf_rule *new_r, *old_r;
	int res;

	res = nf_chain_rule_prepare(chain, r, &rules, &new_r);
	if (res != 0) {
		return res;
	}

	old_r = nf_get_rule_by_num(chain, num);
	if (!old_r) {
		dlist_add_prev(&new_r->lnk, rules);
	} else {
		dlist_add_prev(&new_r->lnk, &old_r->lnk);
	}

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
	struct dlist_head *rules;
	struct nf_rule *r;

	rules = nf_get_chain(chain);
	if (rules == NULL) {
		return -EINVAL;
	}

	dlist_foreach_entry(r, rules, lnk) {
		free_rule(r);
	}

	return 0;
}

#define NF_TEST_NOT_FIELD(test_r, r, field)         \
	(!r->set_##field ? 1 : !test_r->set_##field ? 0 \
		: (assert(!test_r->not_##field),            \
			(0 == memcmp(&test_r->field, &r->field, \
					sizeof test_r->field))          \
				!= !!r->not_##field))

int nf_test_rule(int chain, const struct nf_rule *test_r) {
	struct dlist_head *rules;
	struct nf_rule *r;

	rules = nf_get_chain(chain);
	if (rules == NULL) {
		return -EINVAL;
	}

	if (test_r == NULL) {
		return -EINVAL;
	}

	dlist_foreach_entry(r, rules, lnk) {
		if ((r->target != NF_TARGET_UNKNOWN)
				&& NF_TEST_NOT_FIELD(test_r, r, hwaddr_src)
				&& NF_TEST_NOT_FIELD(test_r, r, hwaddr_dst)
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
				&& (!r->test_hnd ? 1 : r->test_hnd(test_r, r->test_hnd_data))) {

			return test_r->target != r->target;
		}
	}

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
	NF_SET_NOT_FIELD_PTR(&rule, saddr, 0, &test_skb->nh.iph->saddr,
			sizeof test_skb->nh.iph->saddr);
	NF_SET_NOT_FIELD_PTR(&rule, daddr, 0, &test_skb->nh.iph->daddr,
			sizeof test_skb->nh.iph->daddr);
	switch (test_skb->nh.iph->proto) {
	case IPPROTO_ICMP:
		NF_SET_NOT_FIELD(&rule, proto, 0, NF_PROTO_ICMP);
		break;
	case IPPROTO_TCP:
		NF_SET_NOT_FIELD(&rule, proto, 0, NF_PROTO_TCP);
		NF_SET_NOT_FIELD(&rule, sport, 0, test_skb->h.th->source);
		NF_SET_NOT_FIELD(&rule, dport, 0, test_skb->h.th->dest);
		break;
	case IPPROTO_UDP:
		NF_SET_NOT_FIELD(&rule, proto, 0, NF_PROTO_UDP);
		NF_SET_NOT_FIELD(&rule, sport, 0, test_skb->h.uh->source);
		NF_SET_NOT_FIELD(&rule, dport, 0, test_skb->h.uh->dest);
		break;
	}

	return nf_test_rule(chain, &rule);
}

int nf_test_raw(int chain, enum nf_target target, const void *hwaddr_dst,
		const void *hwaddr_src, size_t hwaddr_len) {

	struct nf_rule rule;

	nf_rule_init(&rule);

	rule.target = target;
	NF_SET_NOT_FIELD_PTR(&rule, hwaddr_src, 0, hwaddr_src,
			hwaddr_len);
	NF_SET_NOT_FIELD_PTR(&rule, hwaddr_dst, 0, hwaddr_dst,
			hwaddr_len);

	return nf_test_rule(chain, &rule);
}
