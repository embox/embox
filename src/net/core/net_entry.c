/**
 * @file
 * @brief
 *
 * @date 27.10.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <mem/misc/pool.h>
#include <net/if.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <embox/unit.h>

extern int __netif_receive_skb(sk_buff_t *skb);

int netif_receive_skb(sk_buff_t *skb) {
	return __netif_receive_skb(skb);
}
