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

#define NET_RX_DEBUG 0
#if NET_RX_DEBUG
#include <kernel/printk.h>
#define DBG(x) x
#else
#define DBG(x)
#endif

int net_rx(struct sk_buff *skb) {
	struct net_header_info hdr_info;
	const struct net_pack *npack;

	/* check L2 header size */
	assert(skb != NULL);
	assert(skb->dev != NULL);
	if (skb->len < skb->dev->hdr_len) {
		DBG(printk("net_rx: %p invalid length %zu\n", skb,
					skb->len));
		skb_free(skb);
		return 0; /* error: invalid size */
	}

	/* parse L2 header */
	assert(skb->dev->ops != NULL);
	assert(skb->dev->ops->parse_hdr != NULL);
	if (0 != skb->dev->ops->parse_hdr(skb, &hdr_info)) {
		DBG(printk("net_rx: %p can't parse header\n", skb));
		skb_free(skb);
		return 0; /* error: can't parse L2 header */
	}

	/* check recipient on L2 layer */
	switch (pkt_type(skb)) {
	default:
		DBG(printk("net_rx: %p not for us\n", skb));
		skb_free(skb);
		return 0; /* ok, but: not for us */
	case PACKET_HOST:
	case PACKET_LOOPBACK:
	case PACKET_BROADCAST:
	case PACKET_MULTICAST:
		break;
	}

	/* lookup handler for L3 layer */
	npack = net_pack_lookup(hdr_info.type);
	if (npack == NULL) {
		DBG(printk("net_rx: %p unknown type %#.6hx\n", skb,
					hdr_info.type));
		skb_free(skb);
		return 0; /* ok, but: not supported */
	}

	/* setup L3 header */
	assert(skb->mac.raw != NULL);
	skb->nh.raw = skb->mac.raw + skb->dev->hdr_len;

	DBG(printk("net_rx: %p len %zu type %#.6hx\n",
				skb, skb->len, hdr_info.type));

	/* decrypt packet */
	skb = net_decrypt(skb);
	if (skb == NULL) {
		return 0; /* error: something wrong :( */
	}

	sock_packet_add(skb);

	/* handling on L3 layer */
	return npack->rcv_pack(skb, skb->dev);
}
