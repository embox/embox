/**
 * @file
 * @brief The Internet Protocol (IP) module.
 *
 * @date 16.05.13
 * @author Ilia Vaprol
 */

#include <framework/net/proto/api.h>
#include <net/skbuff.h>
#include <embox/net/pack.h>
#include <net/l3/ipv6.h>
#include <net/netdevice.h>
#include <arpa/inet.h>

EMBOX_NET_PACK(ETH_P_IPV6, ip6_rcv);

#include <kernel/printk.h>
static int ip6_rcv(struct sk_buff *skb, struct net_device *dev) {
	ip6hdr_t *ip6h = ip6_hdr(skb);
	const struct net_proto *nproto;
	unsigned short len;

	if (ip6h->version != 6) {
		dev->stats.rx_err++;
		skb_free(skb);
		return 0; /* error: invalid hdr */
	}

	len = ntohs(ip6h->payload_len);
	if (len + IP6_HEADER_SIZE > skb->len) {
		dev->stats.rx_length_errors++;
		skb_free(skb);
		return 0; /* error: invalid length */
	}

	/* Setup transport layer header */
	skb->h.raw = skb->nh.raw + IP6_HEADER_SIZE;

	nproto = net_proto_lookup(ETH_P_IPV6, ip6h->nexthdr);
	if (nproto != NULL) {
		return nproto->handle(skb);
	}

//	printk("ipv6 packet accepted, %#x\n", ip6h->nexthdr);

	skb_free(skb);
	return 0; /* error: nobody wants this packet */
}
