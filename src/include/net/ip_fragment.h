/**
 * @date 25.11.2011
 * @author Alexander Kalmuk
 */

#ifndef NET_IP_FRAGMENT_H
#define NET_IP_FRAGMENT_H

#include <types.h>
#include <util/list.h>
#include <net/in.h>
#include <net/ip.h>
#include <net/skbuff.h>
#include <util/macro.h>

#define MAX_BUFS_CNT 0x10      /* Maximum uncomplete packets count */
#define MSL          2*60*1000 /* Maximum Segment Lifetime is 2 minutes */

/**
 *	return sk_buff containing complete data
 */
extern struct sk_buff *ip_defrag(struct sk_buff *skb);

/* When skb is large then interface MTU we split it into
 * number of smaller pieces. They are linked into sk_buff_head.
 * We return NULL if we don't have enough memory.
 * During this operation we don't touch the original skb
 */
extern struct sk_buff_head *ip_frag(const struct sk_buff *skb, uint32_t mtu);

#endif /* NET_IP_FRAGMENT_H */
