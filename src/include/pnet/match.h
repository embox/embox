/**
 * @date 24.10.11
 * @Alexander Kalmuk
 */
#ifndef NET_MATCH_H_
#define NET_MATCH_H_

#include <pnet/types.h>

#define MAX_RULE_COUNT 0x10

extern net_node_t create_matcher(net_hnd rx_matcher, net_hnd tx_matcher);
extern int add_new_rule(match_rule_t *rule, net_node_t node);
extern int hwaddrs_rule_create(char *h_dest, net_node_t node);
extern int match_hwaddrs(net_packet_t packet);

#endif /* NET_MATCH_H_ */
