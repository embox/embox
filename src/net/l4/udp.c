/**
 * @file
 * @brief The User Datagram Protocol (UDP).
 * @details RFC 768
 *
 * @date 26.03.09
 * @author Nikolay Korotky
 */

#include <stdlib.h>
#include <embox/net/proto.h>
#include <net/l3/ipv4/ip.h>
#include <net/l4/udp.h>
#include <net/l3/icmpv4.h>
#include <assert.h>
#include <errno.h>
#include <net/socket/inet_sock.h>
#include <arpa/inet.h>
#include <net/netdevice.h>
#include <framework/net/sock/api.h>

static void udp_err(sk_buff_t *skb, unsigned int info);

EMBOX_NET_PROTO(IPPROTO_UDP, udp_rcv, udp_err);

static int udp_rcv_tester(const struct sock *sk,
		const struct sk_buff *skb) {
	const struct inet_sock *inet_sk;

	inet_sk = (const struct inet_sock *)sk;
	assert(inet_sk != NULL);

	assert(skb != NULL);
	assert(skb->nh.iph != NULL);
	assert(skb->h.uh != NULL);
	return ((inet_sk->rcv_saddr == INADDR_ANY)
				|| (inet_sk->rcv_saddr == skb->nh.iph->daddr))
			&& (inet_sk->sport == skb->h.uh->dest);
}

static int udp_rcv(struct sk_buff *skb) {
	struct sock *sk;
	struct inet_sock *inet;
	int res;

	assert(skb != NULL);

	sk = sock_lookup(NULL, udp_sock_ops,
			udp_rcv_tester, skb);

#if 0
	/* FIXME for bootp; use raw socket */
	if (sk == NULL) {
		sk = udp_lookup(0, udph->dest);
	}
#endif

	if (sk != NULL) {
		inet = inet_sk(sk);
		inet->dport = skb->h.uh->source;
		inet->daddr = skb->nh.iph->saddr;

		if (sk->sk_encap_rcv) {
			if (0 > (res = sk->sk_encap_rcv(sk, skb)))
				return -res;
		}

		sock_rcv(sk, skb, skb->h.raw + UDP_HEADER_SIZE);

		if (inet->rcv_saddr == INADDR_ANY) {
			//TODO: temporary
			inet->saddr = skb->nh.iph->daddr;
		}
	} else {
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0);
	}
	return ENOERR;
}

static int udp_err_tester(const struct sock *sk,
		const struct sk_buff *skb) {
	const struct inet_sock *inet_sk;
	const struct iphdr *emb_pack_iphdr;
	const struct udphdr *emb_pack_udphdr;

	inet_sk = (const struct inet_sock *)sk;
	assert(inet_sk != NULL);

	assert(skb != NULL);
	assert(skb->h.raw != NULL);
	emb_pack_iphdr = (const struct iphdr *)(skb->h.raw
			+ IP_HEADER_SIZE(skb->nh.iph) + ICMP_HEADER_SIZE);

	assert(skb->nh.raw != NULL);
	emb_pack_udphdr = (const struct udphdr *)(skb->h.raw
			+ IP_HEADER_SIZE(skb->nh.iph) + ICMP_HEADER_SIZE
			+ IP_HEADER_SIZE(emb_pack_iphdr));

	return !(inet_sk->daddr != emb_pack_iphdr->saddr && inet_sk->daddr)
			&& !(inet_sk->rcv_saddr != emb_pack_iphdr->daddr && inet_sk->rcv_saddr)
			&& (inet_sk->sport == emb_pack_udphdr->dest)
			&& (inet_sk->dport == emb_pack_udphdr->source)
			/* sk_it->sk_bound_dev_if struct sock doesn't have device binding? */
			&& inet_sk->sk.opt.so_protocol == emb_pack_iphdr->proto;
}

static void udp_err(sk_buff_t *skb, unsigned int info) {
	struct sock *sk;

	sk = NULL;

	/* notify all sockets matching source, dest address, protocol and ports*/
	while (1) {
		sk = sock_lookup(sk, udp_sock_ops, udp_err_tester, skb);
		if (sk == NULL) {
			break;
		}

		/* notify socket about an error */
		ip_v4_icmp_err_notify(sk, skb->h.icmph->type, skb->h.icmph->code);
	}
}
