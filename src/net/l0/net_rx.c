/**
 * @file
 * @brief
 *
 * @date 19.06.11
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>

#include <embox/net/pack.h>
#include <net/if_packet.h>
#include <net/l0/net_crypt.h>
#include <net/l0/net_rx.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/socket/packet.h>
#include <util/log.h>

#define LOG_LEVEL OPTION_GET(NUMBER, log_level)

int net_rx(struct sk_buff *skb) {
	struct net_header_info hdr_info;
	const struct net_pack *npack;

	/* check L2 header size */
	assert(skb != NULL);
	assert(skb->dev != NULL);
	if (skb->len < skb->dev->hdr_len) {
		log_error("%p invalid length %zu", skb, skb->len);
		skb_free(skb);
		return 0; /* error: invalid size */
	}

	/* parse L2 header */
	assert(skb->dev->ops != NULL);
	assert(skb->dev->ops->parse_hdr != NULL);
	if (0 != skb->dev->ops->parse_hdr(skb, &hdr_info)) {
		log_error("%p can't parse header", skb);
		skb_free(skb);
		return 0; /* error: can't parse L2 header */
	}

	/* check recipient on L2 layer */
	switch (pkt_type(skb)) {
	default:
		log_debug("%p not for us", skb);
		skb_free(skb);
		return 0; /* ok, but: not for us */
	case PACKET_HOST:
	case PACKET_LOOPBACK:
	case PACKET_BROADCAST:
	case PACKET_MULTICAST:
		break;
	}

	/* setup L3 header */
	assert(skb->mac.raw != NULL);
	skb->nh.raw = skb->mac.raw + skb->dev->hdr_len;

	log_debug("%p len %zu type %#.6hx", skb, skb->len, hdr_info.type);

	/* decrypt packet */
	skb = net_decrypt(skb);
	if (skb == NULL) {
		return 0; /* error: something wrong :( */
	}

	sock_packet_add(skb, hdr_info.type);

	/* lookup handler for L3 layer
	 * We check if L3 handler exists only after sock_packet_add(), because of
	 * we must pass skb to all packet sockets even though L3 header is not valid
	 * from Embox kernel's point of view. */
	npack = net_pack_lookup(hdr_info.type);
	if (npack == NULL) {
		log_debug("%p unknown type %#.6hx", skb, hdr_info.type);
		skb_free(skb);
		return 0; /* ok, but: not supported */
	}

	/* handling on L3 layer */
	return npack->rcv_pack(skb, skb->dev);
}
