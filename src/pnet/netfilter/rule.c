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
#include <pnet/netfilter/match_lin.h>

OBJALLOC_DEF(match_rules, struct match_rule, MAX_RULE_COUNT);

static void rule_init(match_rule_t rule) {
	rule->id = 0;

	rule->skbuf->data = rule->header;

	/*  Link layer header allocate in rule */
	rule->skbuf->mac.raw = (unsigned char*) rule->header;

	/* Network layer header allocate in rule */
	rule->skbuf->nh.raw = (unsigned char*) rule->header + ETH_HEADER_SIZE;
}

match_rule_t pnet_rule_alloc(void) {
	match_rule_t rule = (match_rule_t) objalloc(&match_rules);

	memset(rule, MATCH_WILDCARD, sizeof(struct match_rule));

	rule_init(rule);

	return rule;
}

