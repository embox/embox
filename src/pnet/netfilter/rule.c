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

OBJALLOC_DEF(match_rules, match_rule_t, MAX_RULE_COUNT);

match_rule_t pnet_rule_alloc(void) {
	match_rule_t rule = (match_rule_t) objalloc(&match_rules);

	memset(rule, MATCH_WILDCARD, sizeof(struct match_rule));

	return rule;
}

