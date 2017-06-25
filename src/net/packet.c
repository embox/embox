/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.07.2014
 */

#include <assert.h>
#include <string.h>
#include <linux/etherdevice.h>
#include <net/l2/ethernet.h>
#include <net/if.h>
#include <net/netdevice.h>
#include <net/skbuff.h>

#include <net/if_packet.h>

int pkt_type(const struct sk_buff *skb) {
	assert(skb != NULL);
	assert(skb->dev != NULL);
	assert(skb->mac.ethh != NULL);

	/* it's loopback? */
	if (skb->dev->flags & IFF_LOOPBACK) {
		return PACKET_LOOPBACK;
	}

	/* it's multicast/broadcast? */
	if (is_multicast_ether_addr(&skb->mac.ethh->h_dest[0])) {
		if (0 == compare_ether_addr(&skb->mac.ethh->h_dest[0],
					&skb->dev->broadcast[0])) {
			return PACKET_BROADCAST;
		}
		else {
			return PACKET_MULTICAST;
		}
	}

	/* it's for me? */
	if (0 == compare_ether_addr(&skb->mac.ethh->h_dest[0],
				&skb->dev->dev_addr[0])) {
		return PACKET_HOST;
	}

	/* it's from me? */
	if (0 == compare_ether_addr(&skb->mac.ethh->h_source[0],
				&skb->dev->dev_addr[0])) {
		return PACKET_OUTGOING;
	}

	/* it's not for me :( */
	return PACKET_OTHERHOST;
}

