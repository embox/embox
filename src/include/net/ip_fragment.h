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
#define MTU          1500        /* Maximum Transmission Unit is 1500 bytes */
/**
 *	return sk_buff containing complete data
 */
extern struct sk_buff *ip_defrag(struct sk_buff *skb);
extern struct sk_buff_head *ip_frag(struct sk_buff *skb);

#endif /* NET_IP_FRAGMENT_H */
