/**
 * @file
 * @brief
 *
 * @date 19.06.11
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <framework/net/pack/api.h>
#include <net/if_packet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>

int net_rx(struct sk_buff *skb) {
	struct net_header_info hdr_info;
	const struct net_pack *npack;

	/* parse L2 header */
	assert(skb != NULL);
	assert(skb->dev != NULL);
	assert(skb->dev->ops != NULL);
	assert(skb->dev->ops->parse_hdr != NULL);
	if (0 != skb->dev->ops->parse_hdr(skb, &hdr_info)) {
		return 0; /* error: can't parse L2 header */
	}

	/* check recipient on L2 layer */
	switch (pkt_type(skb)) {
	default:
		return 0; /* not for us */
	case PACKET_HOST:
	case PACKET_LOOPBACK:
	case PACKET_BROADCAST:
	case PACKET_MULTICAST:
		break;
	}

	/* lookup handler for L3 layer */
	npack = net_pack_lookup(hdr_info.type);
	if (npack == NULL) {
		skb_free(skb);
		return 0; /* not supported */
	}

	/* handling on L3 layer */
	return npack->handle(skb, skb->dev);
}
