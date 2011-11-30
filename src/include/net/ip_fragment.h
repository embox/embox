/**
 * @date 25.11.2011
 */

#ifndef NET_IP_FRAGMENT_H
#define NET_IP_FRAGMENT_H

#include <types.h>
#include <util/list.h>
#include <net/in.h>
#include <net/ip.h>
#include <net/skbuff.h>
#include <util/macro.h>

#define MAX_BUFS_CNT 0x10
/**
 *	return skbuff contain complete data
 */
extern struct sk_buff *ip_defrag(struct sk_buff *skb);

#endif /* NET_IP_FRAGMENT_H */
