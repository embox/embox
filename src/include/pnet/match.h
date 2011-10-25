/**
 * @date 24.10.11
 * @Alexander Kalmuk
 */
#ifndef NET_MATCH_H_
#define NET_MATCH_H_

#include <pnet/types.h>

#define MAX_RULE_COUNT 0x10

extern int hwaddrs_rool_create(unsigned char *h_dest);
extern int add_new_rule(match_rule_t *rule);
extern int match_hwaddrs(struct sk_buff *skb);

#endif /* NET_MATCH_H_ */
