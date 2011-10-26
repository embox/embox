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
	int n = ETH_ALEN;

	rule->dest_ip = UINT_MAX;
	rule->src_ip = UINT_MAX;

	while (n--) {
		rule->ether_header.h_dest[n] = 255;
		rule->ether_header.h_source[n] = 255;
	}

	memset(rule, MATCH_WILDCARD, sizeof(struct match_rule));

	return rule;
}

