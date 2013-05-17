/**
 * @file
 * @brief
 *
 * @date 17.05.13
 * @author Ilia Vaprol
 */

#include <netinet/in.h>
#include <stddef.h>
#include <embox/net/proto.h>
#include <net/icmp6.h>
#include <net/skbuff.h>
#include <net/checksum.h>
#include <arpa/inet.h>

EMBOX_NET_PROTO(IPPROTO_ICMPV6, icmp6_rcv, NULL);

#include <kernel/printk.h>
#include <net/netdevice.h>
#include <string.h>
#include <net/ipv6.h>
static int icmp6_hnd_echo_request(struct icmp6hdr *icmp6h,
		struct sk_buff *skb) {
	struct in6_addr in6;
	struct ip6_phdr {
		struct in6_addr saddr;
		struct in6_addr daddr;
		__be32 len;
		__u8   zero[3];
		__u8   hdr;
	} ip6phdr;

	if (icmp6h->code != 0) {
		skb_free(skb);
		return -1;
	}

	/* setup IPv6 header */
	memcpy(&in6, &skb->nh.ip6h->saddr, sizeof in6);
	memcpy(&skb->nh.ip6h->saddr, &skb->nh.ip6h->daddr, sizeof in6);
	memcpy(&skb->nh.ip6h->daddr, &in6, sizeof in6);

	/* setup pseudo-header */
	memcpy(&ip6phdr.saddr, &skb->nh.ip6h->saddr, sizeof in6);
	memcpy(&ip6phdr.daddr, &skb->nh.ip6h->daddr, sizeof in6);
	ip6phdr.len = htonl(ntohs(skb->nh.ip6h->payload_len));
	memset(&ip6phdr.zero[0], 0, sizeof ip6phdr.zero);
	ip6phdr.hdr = skb->nh.ip6h->nexthdr;

	/* setup ICMPv6 header */
	icmp6h->type = ICMP6_ECHO_REPLY;
	icmp6h->checksum = 0;
	icmp6h->checksum = (~fold_short(partial_sum(&ip6phdr, sizeof ip6phdr) +
			partial_sum(icmp6h, ntohs(skb->nh.ip6h->payload_len))) & 0xffff);

	/* setup MAC */
	memcpy(&skb->mac.ethh->h_dest[0], &skb->mac.ethh->h_source[0], skb->dev->addr_len);
	memcpy(&skb->mac.ethh->h_source[0], &skb->dev->dev_addr[0], skb->dev->addr_len);

	return dev_xmit_skb(skb);
}

static int icmp6_rcv(struct sk_buff *skb) {
	struct icmp6hdr *icmp6h = icmp6_hdr(skb);

	switch (icmp6h->type) {
	default:
		printk("icmp6_rcv: unknown type: %hhu\n", icmp6h->type);
		break;
	case ICMP6_ECHO_REQUEST:
		return icmp6_hnd_echo_request(icmp6h, skb);
	case ICMP6_ECHO_REPLY:
		printk("icmp6_rcv: echo-reply\n");
		break;
	}

	skb_free(skb);
	return 0;
}
