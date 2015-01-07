/**
 * @file
 *
 * @date 25.11.2011
 * @author Alexander Kalmuk
 */

#ifndef NET_L3_IPV4_IP_FRAGMENT_H
#define NET_L3_IPV4_IP_FRAGMENT_H

#include <stdint.h>
#include <net/skbuff.h>

#define MSL          2*60*1000 /* Maximum Segment Lifetime is 2 minutes */


struct sk_buff;

/**
 *	return sk_buff containing complete data
 */
extern struct sk_buff *ip_defrag(struct sk_buff *skb);

/* When skb is large then interface MTU we split it into
 * number of smaller pieces. They are linked into sk_buff_head.
 * We return NULL if we don't have enough memory.
 * During this operation we don't touch the original skb
 */
extern int ip_frag(const struct sk_buff *skb, uint32_t mtu,
		struct sk_buff_head *tx_buf);

#endif /* NET_L3_IPV4_IP_FRAGMENT_H */
