/**
 * @file
 * @brief
 *
 * @date 09.05.13
 * @author Ilia Vaprol
 */

#ifndef NET_IF_PACKET_H_
#define NET_IF_PACKET_H_

#include <assert.h>
#include <linux/etherdevice.h>
#include <net/if.h>
#include <net/skbuff.h>

/**
 * Prototypes
 */
struct sk_buff;

/**
 * Packet types
 */
enum {
	PACKET_HOST,      /* To us */
	PACKET_BROADCAST, /* To all */
	PACKET_MULTICAST, /* To group */
	PACKET_OTHERHOST, /* To someone else */
	PACKET_LOOPBACK   /* We are in loopback */
};

/**
 * Determine the paket type
 */
static inline int pkt_type(const struct sk_buff *skb) {
	assert(skb != NULL);
	assert(skb->dev != NULL);
	assert(skb->mac.ethh != NULL);
	return skb->dev->flags & IFF_LOOPBACK ? PACKET_LOOPBACK
			: !is_multicast_ether_addr(&skb->mac.ethh->h_dest[0])
				? !compare_ether_addr(&skb->mac.ethh->h_dest[0],
						&skb->dev->dev_addr[0])
					? PACKET_HOST : PACKET_OTHERHOST
				: !compare_ether_addr(&skb->mac.ethh->h_dest[0],
						&skb->dev->broadcast[0])
					? PACKET_BROADCAST : PACKET_MULTICAST;
}

#endif /* NET_IF_PACKET_H_ */
