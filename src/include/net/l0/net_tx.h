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

extern int net_tx(struct sk_buff *skb,
		struct net_header_info *hdr_info);

#endif /* NET_L0_NET_TX_ */
