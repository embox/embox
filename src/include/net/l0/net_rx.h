/**
 * @file
 * @brief
 *
 * @date 16.07.13
 * @author Ilia Vaprol
 */

#ifndef NET_L0_NET_RX_
#define NET_L0_NET_RX_

#include <net/skbuff.h>

/**
 * This funciton starts stack handling of incoming package
 * @param skb - incoming package which should be handled
 */
extern int net_rx(struct sk_buff *skb);

#endif /* NET_L0_NET_RX_ */
