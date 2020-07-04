/**
 * @file
 * @brief
 *
 * @date 16.07.13
 * @author Ilia Vaprol
 */

#ifndef NET_L0_NET_TX_
#define NET_L0_NET_TX_

#include <net/netdevice.h>
#include <net/skbuff.h>

struct net_header_info;
struct sk_buff;

extern int net_tx(struct sk_buff *skb, struct net_header_info *hdr_info);
extern int net_tx_direct(struct sk_buff *skb);

#endif /* NET_L0_NET_TX_ */
