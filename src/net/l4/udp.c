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
#include <net/ip.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <assert.h>
#include <errno.h>
#include <net/inet_sock.h>
#include <arpa/inet.h>
#include <net/netdevice.h>
#include <framework/net/sock/api.h>

static void udp_err(sk_buff_t *skb, unsigned int info);

EMBOX_NET_PROTO(IPPROTO_UDP, udp_rcv, udp_err);

static struct sock *_udp_v4_lookup(struct sock *sk,
		unsigned char protocol, unsigned int saddr, unsigned int daddr,
		unsigned short sport, unsigned short dport, struct net_device *dev) {
	/* socket for iterating */
	const struct net_sock *udp_nsock = net_sock_lookup(
			AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct inet_sock *inet;

	if (udp_nsock == NULL) {
		return NULL; /* error: udp_sock not suported */
	}

	if (sk == NULL) {
		sk = sock_iter(udp_nsock->ops);
	}

	inet = (struct inet_sock *)sk;
	while (inet != NULL) {
		inet = (struct inet_sock *)sock_next(&inet->sk);
		if (!(inet->daddr != daddr && inet->daddr) &&
		    !(inet->rcv_saddr != saddr && inet->rcv_saddr) &&
		    (inet->sport == sport) && (inet->dport == dport) &&
			 /* sk_it->sk_bound_dev_if struct sock doesn't have device binding? */
			 inet->sk.opt.so_protocol == protocol){
			return &inet->sk;
		}
	}

	return NULL;
}

static struct sock * udp_lookup(in_addr_t daddr, __be16 dport) {
	const struct net_sock *udp_nsock = net_sock_lookup(
			AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct inet_sock *inet;

	if (udp_nsock == NULL) {
		return NULL; /* error: udp_sock not suported */
	}

	for (inet = (struct inet_sock *)sock_iter(udp_nsock->ops);
			inet != NULL;
			inet = (struct inet_sock *)sock_next(&inet->sk)) {
		if (((inet->rcv_saddr == INADDR_ANY) ||
		    (inet->rcv_saddr == daddr)) &&
		    (inet->sport == dport)) {
			return &inet->sk;
		}
	}

	return NULL;
}

static int udp_rcv(struct sk_buff *skb) {
	iphdr_t *iph;
	udphdr_t *udph;
	struct sock *sk;
	struct inet_sock *inet;
	int res;

	assert(skb != NULL);

	iph = ip_hdr(skb);
	udph = udp_hdr(skb);
	sk = udp_lookup(iph->daddr, udph->dest);

	/* FIXME for bootp; use raw socket */
	if (sk == NULL) {
		sk = udp_lookup(0, udph->dest);
	}

	if (sk != NULL) {
		inet = inet_sk(sk);
		inet->dport = udph->source;
		inet->daddr = iph->saddr;

		if (sk->sk_encap_rcv) {
			if (0 > (res = sk->sk_encap_rcv(sk, skb)))
				return -res;
		}

		sock_rcv(sk, skb);

		if (inet->rcv_saddr == INADDR_ANY) {
			//TODO: temporary
			inet->saddr = skb->nh.iph->daddr;
		}
	} else {
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0);
	}
	return ENOERR;
}

static void udp_err(sk_buff_t *skb, unsigned int info) {
	struct sock *sk;
	struct iphdr *emb_pack_iphdr;
	struct udphdr *emb_pack_udphdr;

	/* size_t i; */
	/* __be16 port; */

	emb_pack_iphdr = (struct iphdr *)(skb->h.raw + IP_HEADER_SIZE(skb->nh.iph)
			+ ICMP_HEADER_SIZE);
	emb_pack_udphdr = (struct udphdr *)(skb->h.raw + IP_HEADER_SIZE(skb->nh.iph)
			+ ICMP_HEADER_SIZE + IP_HEADER_SIZE(emb_pack_iphdr));

	/* notify all sockets matching source, dest address, protocol and ports*/
	sk = NULL;
	do {
		sk = _udp_v4_lookup(sk, emb_pack_iphdr->proto, emb_pack_iphdr->saddr,
				emb_pack_iphdr->daddr, emb_pack_udphdr->source, emb_pack_udphdr->dest,
				skb->dev);
		if (sk != NULL) { /* notify socket about an error */
			ip_v4_icmp_err_notify(sk, skb->h.icmph->type, skb->h.icmph->code);
			/* do something else - specific for udp sockets ? */
		}
	} while(sk != NULL);
}
