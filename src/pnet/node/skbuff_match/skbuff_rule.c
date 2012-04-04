/**
 * @file
 * @brief
 *
 * @date 26.10.2011
 * @author Anton Kozlov
 */

#include <string.h>
#include <mem/objalloc.h>
#include <net/udp.h>
#include <net/ip.h>
#include <net/if_ether.h>
#include <net/skbuff.h>

#include <pnet/types.h>
#include <pnet/netfilter/match_lin.h>

OBJALLOC_DEF(match_rules, struct match_rule, MAX_RULE_COUNT);

match_rule_t pnet_rule_alloc(void) {
	struct sk_buff *skb;
	match_rule_t rule = (match_rule_t) objalloc(&match_rules);

	/* alloc memory only for header */
	skb = alloc_skb(MAX_PACK_HEADER_SIZE, 0);

	rule->skbuf = skb;
	/* FIXME mac.raw initialized in alloc_skb. I think nh.raw also is uniquely determined.
	 * But now we initialize it in net/core/net_entry.c. So, temporary I suppose
	 * that is no initialization of nh in alloc_skb() --Alexander */
	rule->skbuf->nh.raw = rule->skbuf->data + ETH_HEADER_SIZE;

	/* Fill data with 255 per byte. It means '*', i.e. any value is suitable. */
	memset(rule->skbuf->data, MATCH_WILDCARD, MAX_PACK_HEADER_SIZE);
	rule->next_node = NULL;
	INIT_LIST_HEAD(&rule->lnk);

	return rule;
}

void pnet_rule_free(match_rule_t rule) {
	if (NULL == rule) {
		return;
	}
	kfree_skb(rule->skbuf);
	objfree(&match_rules, rule);
}
