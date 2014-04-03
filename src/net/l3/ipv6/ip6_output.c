/**
 * @file
 * @brief The Internet Protocol Version 6 (IPV6) module.
 *
 * @date 24.10.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <net/l3/ipv6.h>
#include <net/l2/ethernet.h>
#include <net/inetdevice.h>
#include <net/l0/net_tx.h>
#include <net/skbuff.h>
#include <kernel/printk.h>
#include <util/math.h>
#include <embox/net/pack.h>
#include <net/socket/inet6_sock.h>
#include <string.h>
#include <net/lib/ipv6.h>

static const struct net_pack_out_ops ip6_out_ops_struct;
const struct net_pack_out_ops *const ip6_out_ops
		= &ip6_out_ops_struct;

static int ip6_xmit(struct sk_buff *skb) {
	const struct in6_addr *daddr;
	struct net_header_info hdr_info;

	assert(skb != NULL);
	assert(skb->nh.iph != NULL);
	daddr = &skb->nh.ip6h->daddr;

	hdr_info.type = ETH_P_IPV6;
	hdr_info.src_hw = NULL;
	hdr_info.dst_hw = NULL;

	/* FIXME */
	assert(skb->dev != NULL);
	if (skb->dev->flags & IFF_LOOPBACK) {
		hdr_info.dst_p = NULL;
	}
	else {
		hdr_info.dst_p = daddr;
		hdr_info.p_len = sizeof *daddr;
	}

	return net_tx(skb, &hdr_info);
}

static int ip6_make(const struct sock *sk,
		const struct sockaddr *to,
		size_t *data_size, struct sk_buff **out_skb) {
	size_t hdr_size, max_size;
	struct sk_buff *skb;
	struct net_device *dev;
	const struct inet6_sock *in6_sk;
	const struct sockaddr_in6 *to_in6;
	uint8_t nexthdr;
	const struct in6_addr *src_ip6;
	struct in6_addr dst_ip6;

	assert((to == NULL) || (to->sa_family == AF_INET6));
	assert(data_size != NULL);
	assert(out_skb != NULL);
	assert((sk != NULL) || (*out_skb != NULL));

	in6_sk = to_const_inet6_sock(sk);
	to_in6 = (const struct sockaddr_in6 *)to;

	assert((to_in6 == NULL)
			|| (to_in6->sin6_family == AF_INET6));

	memcpy(&dst_ip6, to_in6 != NULL ? &to_in6->sin6_addr
				: in6_sk != NULL ? &in6_sk->dst_in6.sin6_addr
				: &(*out_skb)->nh.ip6h->saddr, /* make a reply */
			sizeof dst_ip6);
	/* FIXME use route */
	if (0 == memcmp(&dst_ip6, &in6addr_loopback,
				sizeof dst_ip6)) {
		dev = netdev_get_by_name("lo");
	}
	else {
		dev = netdev_get_by_name("eth0");
	}
	assert(dev != NULL);

	assert(inetdev_get_by_dev(dev) != NULL);
	src_ip6 = &inetdev_get_by_dev(dev)->ifa6_address;

	nexthdr = in6_sk != NULL ? in6_sk->sk.opt.so_protocol
			: (*out_skb)->nh.ip6h->nexthdr;

	hdr_size = dev->hdr_len + IP6_HEADER_SIZE;
	max_size = min(dev->mtu, skb_max_size());
	if (hdr_size > max_size) {
		return -EMSGSIZE;
	}

	*data_size = min(*data_size, max_size - hdr_size);

	skb = skb_realloc(hdr_size + *data_size, *out_skb);
	if (skb == NULL) {
		return -ENOMEM;
	}

	skb->dev = dev;
	skb->nh.raw = skb->mac.raw + dev->hdr_len;
	skb->h.raw = skb->nh.raw + IP6_HEADER_SIZE;

	ip6_build(skb->nh.ip6h, *data_size, nexthdr, 64, src_ip6,
			&dst_ip6);

	*out_skb = skb;

	return 0;
}

static int ip6_snd(struct sk_buff *skb) {
	return ip6_xmit(skb);
}

static const struct net_pack_out_ops ip6_out_ops_struct = {
	.make_pack = &ip6_make,
	.snd_pack = &ip6_snd
};
