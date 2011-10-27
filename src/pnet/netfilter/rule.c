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

#include <pnet/types.h>
#include <pnet/match.h>

OBJALLOC_DEF(match_rules, match_rule_t, MAX_RULE_COUNT);

static void init_rule(match_rule_t rule) {
	rule->skbuf->data = rule->header;

	/*  Link layer header allocate in rule */
	rule->skbuf->mac.raw = (unsigned char*) rule->header;

	/* Network layer header allocate in rule */
	rule->skbuf->nh.raw = (unsigned char*) rule->header + ETH_HEADER_SIZE;

	/*Transport layer header allocate in rule */
	rule->skbuf->h.raw = rule->skbuf->nh.raw
			+ IP_HEADER_SIZE(rule->skbuf->nh.iph);

}

match_rule_t pnet_rule_alloc(void) {
	match_rule_t rule = (match_rule_t) objalloc(&match_rules);

	memset(rule, MATCH_WILDCARD, sizeof(struct match_rule));

	init_rule(rule);

	return rule;
}

