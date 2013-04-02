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

#define MODOPS_NETFILTER_AMOUNT_RULES  OPTION_GET(NUMBER, amount_rules)

POOL_DEF(nf_rule_pool, struct nf_rule, MODOPS_NETFILTER_AMOUNT_RULES);

LIST_DEF(nf_input_rules);
LIST_DEF(nf_forward_rules);
LIST_DEF(nf_output_rules);

static struct nf_rule * nf_lookup(struct list *rules,
		const struct nf_inet_addr *addr) {
	int addr_is_eq;
	struct nf_rule *r;

	assert((rules != NULL) && (addr != NULL));

	list_foreach(r, rules, lnk) {
		addr_is_eq = memcmp(addr, &r->addr, sizeof *addr) == 0;
		if (!(addr_is_eq ^ !r->not_addr)) {
			return r;
		}
	}

	return NULL;
}

int nf_add_rule(int type, const struct nf_rule *r,
		int overwrite) {
	struct nf_rule *new_r;
	struct list *rules;

	assert(((type == NF_CHAIN_INPUT) || (type == NF_CHAIN_FORWARD)
			|| (type == NF_CHAIN_OUTPUT)) && (r != NULL));

	rules = type == NF_CHAIN_INPUT ? &nf_input_rules
			: type == NF_CHAIN_FORWARD ? &nf_forward_rules
			: &nf_output_rules;

	new_r = nf_lookup(rules, &r->addr);
	if (new_r == NULL) {
		new_r = pool_alloc(&nf_rule_pool);
		if (new_r == NULL) {
			return -ENOMEM;
		}
		list_link_init(&new_r->lnk);
	}
	else if (overwrite) {
		list_unlink_element(new_r, lnk);
	}
	else {
		return -EEXIST;
	}

	new_r->policy = r->policy;
	new_r->not_addr = r->not_addr;
	memcpy(&new_r->addr, &r->addr, sizeof(struct nf_inet_addr));

	list_add_last_element(new_r, rules, lnk);

	return 0;
}

int nf_del_rule(int type, const struct nf_rule *r) {
	struct nf_rule *rule;
	struct list *rules;

	assert(((type == NF_CHAIN_INPUT) || (type == NF_CHAIN_FORWARD)
			|| (type == NF_CHAIN_OUTPUT)) && (r != NULL));

	rules = type == NF_CHAIN_INPUT ? &nf_input_rules
			: type == NF_CHAIN_FORWARD ? &nf_forward_rules
			: &nf_output_rules;

	rule = nf_lookup(rules, &r->addr);
	if (rule == NULL) {
		return -ENOENT;
	}

	list_unlink_element(rule, lnk);
	pool_free(&nf_rule_pool, rule);

	return 0;
}

int nf_clear(void) {
	struct nf_rule *r;

	list_foreach(r, &nf_input_rules, lnk) {
		list_unlink_element(r, lnk);
		pool_free(&nf_rule_pool, r);
	}

	list_foreach(r, &nf_forward_rules, lnk) {
		list_unlink_element(r, lnk);
		pool_free(&nf_rule_pool, r);
	}

	list_foreach(r, &nf_output_rules, lnk) {
		list_unlink_element(r, lnk);
		pool_free(&nf_rule_pool, r);
	}

	return 0;
}

int nf_valid_skb(int type, const struct sk_buff *skb) {
	struct list *rules;
	struct nf_inet_addr saddr;
	struct nf_rule *r;

	assert(((type == NF_CHAIN_INPUT) || (type == NF_CHAIN_FORWARD)
			|| (type == NF_CHAIN_OUTPUT)) && (skb != NULL));

	saddr.in.s_addr = skb->nh.iph->saddr;
	rules = type == NF_CHAIN_INPUT ? &nf_input_rules
			: type == NF_CHAIN_FORWARD ? &nf_forward_rules
			: &nf_output_rules;


	r = nf_lookup(rules, &saddr);
	if (r == NULL) {
		return 1;
	}

	return r->policy == NF_POLICY_ACCEPT ? 1 : 0;
}
