/**
 * @file
 * @brief
 *
 * @date 26.10.2011
 * @author Anton Kozlov
 */

#include <string.h>
#include <mem/objalloc.h>

#include <pnet/types.h>
#include <pnet/match.h>

#define	UINT_MAX 4294967295	//2^32-1
OBJALLOC_DEF(match_rules, match_rule_t, MAX_RULE_COUNT);

match_rule_t pnet_rule_alloc(void) {
	match_rule_t rule = (match_rule_t) objalloc(&match_rules);

	rule->dest_ip = UINT_MAX;
	rule->src_ip = UINT_MAX;

	memset(rule->ether_header.h_dest, 0x00, ETH_ALEN);
	memset(rule->ether_header.h_source, 0x00, ETH_ALEN);

	memset(rule, MATCH_WILDCARD, sizeof(struct match_rule));

	return rule;
}

