/**
 * @file
 * @brief
 *
 * @date 26.10.2011
 * @author Anton Kozlov
 */

#include <string.h>
#include <mem/objalloc.h>
#include <net/l4/udp.h>
#include <net/l3/ipv4/ip.h>
#include <net/l2/ethernet.h>
#include <net/skbuff.h>

#include <pnet/types.h>
#include <pnet/netfilter/match_lin.h>

OBJALLOC_DEF(match_rules, struct match_rule, MAX_RULE_COUNT);

match_rule_t pnet_rule_alloc(void) {
	struct sk_buff *skb;
	match_rule_t rule = (match_rule_t) objalloc(&match_rules);

	/* alloc memory only for header */
	skb = skb_alloc(MAX_PACK_HEADER_SIZE);

	rule->skbuf = skb;
	/* FIXME mac.raw initialized in skb_alloc. I think nh.raw also is uniquely determined.
	 * But now we initialize it in net/core/net_entry.c. So, temporary I suppose
	 * that is no initialization of nh in skb_alloc() --Alexander */
	rule->skbuf->nh.raw = rule->skbuf->mac.raw + ETH_HEADER_SIZE;

	/* Fill data with 255 per byte. It means '*', i.e. any value is suitable. */
	memset(rule->skbuf->mac.raw, MATCH_WILDCARD, MAX_PACK_HEADER_SIZE);
	rule->next_node = NULL;

	/* All rules and packets have priority 0 after allocation. */
	rule->priority = 0;
	INIT_LIST_HEAD(&rule->lnk);

	return rule;
}

void pnet_rule_free(match_rule_t rule) {
	if (NULL == rule) {
		return;
	}
	skb_free(rule->skbuf);
	objfree(&match_rules, rule);
}
