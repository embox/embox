/**
 * @file
 * @brief The User Datagram Protocol (UDP).
 * @details RFC 768
 *
 * @date 26.03.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <embox/net/proto.h>
#include <net/l3/ipv4/ip.h>
#include <net/l3/ipv6.h>
#include <net/l4/udp.h>
#include <net/lib/udp.h>
#include <net/l3/icmpv4.h>
#include <net/l2/ethernet.h>
#include <assert.h>
#include <errno.h>
#include <net/socket/inet_sock.h>
#include <arpa/inet.h>
#include <net/netdevice.h>
#include <framework/mod/options.h>
#include <framework/net/sock/api.h>

#include <net/lib/ipv4.h>
#include <net/lib/ipv6.h>

#define MODOPS_VERIFY_CHKSUM OPTION_GET(BOOLEAN, verify_chksum)

EMBOX_NET_PROTO(ETH_P_IP, IPPROTO_UDP, udp_rcv, udp_err);
EMBOX_NET_PROTO(ETH_P_IPV6, IPPROTO_UDP, udp_rcv,
		net_proto_handle_error_none);

static int udp4_rcv_tester(const struct sock *sk,
		const struct sk_buff *skb) {
	assert(sk != NULL);
	return (sk->opt.so_domain == AF_INET)
			&& ip_tester_dst_or_any(sk, skb)
			&& (sock_inet_get_src_port(sk) == udp_hdr(skb)->dest);
}

static int udp6_rcv_tester(const struct sock *sk,
		const struct sk_buff *skb) {
	assert(sk != NULL);
	return (sk->opt.so_domain == AF_INET6)
			&& ip6_tester_dst_or_any(sk, skb)
			&& (sock_inet_get_src_port(sk) == udp_hdr(skb)->dest);
}

static int udp4_accept_dst(const struct sock *sk,
		const struct sk_buff *skb) {
	assert(sk != NULL);
	return (sk->opt.so_domain == AF_INET)
			&& ip_tester_src_or_any(sk, skb)
			&&  ((sock_inet_get_dst_port(sk)
					== udp_hdr(skb)->source)
				|| (sock_inet_get_dst_port(sk) == 0))
			&& ((sk->opt.so_bindtodevice == skb->dev)
				|| (sk->opt.so_bindtodevice == NULL));
}

static int udp6_accept_dst(const struct sock *sk,
		const struct sk_buff *skb) {
	assert(sk != NULL);
	return (sk->opt.so_domain == AF_INET6)
			&& ip6_tester_src_or_any(sk, skb)
			&&  ((sock_inet_get_dst_port(sk)
					== udp_hdr(skb)->source)
				|| (sock_inet_get_dst_port(sk) == 0))
			&& ((sk->opt.so_bindtodevice == skb->dev)
				|| (sk->opt.so_bindtodevice == NULL));
}

static int udp_rcv(struct sk_buff *skb) {
	struct sock *sk;

	assert(skb != NULL);
	assert(ip_check_version(ip_hdr(skb))
			|| ip6_check_version(ip6_hdr(skb)));

	/* Check CRC */
	if (MODOPS_VERIFY_CHKSUM) {
		uint16_t old_check;
		old_check = skb->h.uh->check;
		udp_set_check_field(skb->h.uh, skb->nh.raw);
		if (old_check != skb->h.uh->check) {
			return 0; /* error: bad checksum */
		}
	}

	sk = sock_lookup(NULL, udp_sock_ops,
			ip_check_version(ip_hdr(skb))
				? udp4_rcv_tester : udp6_rcv_tester,
			skb);
	if (sk != NULL) {
		if (ip_check_version(ip_hdr(skb))
				? udp4_accept_dst(sk, skb)
				: udp6_accept_dst(sk, skb)) {
			sock_rcv(sk, skb, skb->h.raw + UDP_HEADER_SIZE,
					udp_data_length(udp_hdr(skb)));
		}
		else {
			skb_free(skb);
		}
	}
	else {
		icmp_discard(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH);
	}

	return 0;
}

static int udp_err_tester(const struct sock *sk,
		const struct sk_buff *skb) {
	const struct inet_sock *in_sk;
	const struct iphdr *emb_pack_iphdr;
	const struct udphdr *emb_pack_udphdr;

	in_sk = (const struct inet_sock *)sk;
	assert(in_sk != NULL);
	assert(in_sk->src_in.sin_family == AF_INET);
	assert(in_sk->dst_in.sin_family == AF_INET);

	assert(skb != NULL);
	assert(skb->h.raw != NULL);
	emb_pack_iphdr = (const struct iphdr *)(skb->h.raw
			+ IP_HEADER_SIZE(skb->nh.iph) + ICMP_MIN_HEADER_SIZE);

	assert(skb->nh.raw != NULL);
	emb_pack_udphdr = (const struct udphdr *)(skb->h.raw
			+ IP_HEADER_SIZE(skb->nh.iph) + ICMP_MIN_HEADER_SIZE
			+ IP_HEADER_SIZE(emb_pack_iphdr));

	return (((in_sk->src_in.sin_addr.s_addr == skb->nh.iph->daddr)
					&& (in_sk->src_in.sin_addr.s_addr == emb_pack_iphdr->saddr))
				|| (in_sk->src_in.sin_addr.s_addr == INADDR_ANY))
			&& (in_sk->src_in.sin_port == emb_pack_udphdr->source)
			&& (((in_sk->dst_in.sin_addr.s_addr == skb->nh.iph->saddr)
					&& (in_sk->dst_in.sin_addr.s_addr == emb_pack_iphdr->daddr))
				|| (in_sk->dst_in.sin_addr.s_addr == INADDR_ANY))
			&& ((in_sk->dst_in.sin_port == emb_pack_udphdr->dest)
				|| (in_sk->dst_in.sin_port == 0))
			&& (in_sk->sk.opt.so_protocol == emb_pack_iphdr->proto)
			&& ((in_sk->sk.opt.so_bindtodevice == skb->dev)
				|| (in_sk->sk.opt.so_bindtodevice == NULL));
}

static void udp_err(const struct sk_buff *skb, int error_info) {
	struct sock *sk;

	sk = NULL;

	while (1) {
		sk = sock_lookup(sk, udp_sock_ops, udp_err_tester, skb);
		if (sk == NULL) {
			break;
		}

		sock_set_so_error(sk, error_info);
	}
}
