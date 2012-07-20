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
#include <net/in.h>
#include <net/netdevice.h>

EMBOX_NET_PROTO(IPPROTO_UDP, udp_rcv, udp_err);

struct udp_sock *udp_table[CONFIG_MAX_KERNEL_SOCKETS];

/* static method for getting socket index in hash table */
static int _udp_v4_get_hash_idx(struct sock *sk) {
	unsigned int i;

	assert(sk != NULL);

	for (i = 0; i < sizeof udp_table / sizeof udp_table[0]; ++i) {
		if (&udp_table[i]->inet.sk == sk) {
			return i;
		}
	}

	return -1;
}

/* static method for looking an sk by index, proto, sourace and dest addresses and ports */
static struct sock *_udp_v4_lookup(unsigned int sk_hash_idx,
		unsigned char protocol, unsigned int saddr, unsigned int daddr,
		unsigned short sport, unsigned short dport, struct net_device *dev) {
	/* socket for iterating */
	struct sock * sk_it;
	struct inet_sock *inet;
	int i;

	for (i = sk_hash_idx; i < sizeof udp_table / sizeof udp_table[0]; ++i) {
		sk_it = &udp_table[i]->inet.sk;
		inet = inet_sk(sk_it);
		/* the socket is being searched for by (daddr, saddr, protocol) */
		if (!(inet->daddr != daddr && inet->daddr) &&
		    !(inet->rcv_saddr != saddr && inet->rcv_saddr) &&
		    (inet->sport == sport) && (inet->dport == dport) &&
			 /* sk_it->sk_bound_dev_if struct sock doesn't have device binding? */
			 sk_it->sk_protocol == protocol){
			return sk_it;
		}
	}

	return NULL;
}

static struct sock * udp_lookup(in_addr_t daddr, __be16 dport) {
	struct inet_sock *inet;
	size_t i;

	for (i = 0; i< sizeof udp_table / sizeof udp_table[0]; ++i) {
		inet = (struct inet_sock *)udp_table[i];
		if (inet != NULL) {
			if (((inet->rcv_saddr == INADDR_ANY) ||
			    (inet->rcv_saddr == daddr)) &&
			    (inet->sport == dport)) {
				return (struct sock *)inet;
			}
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
	if (sk != NULL) {
		inet = inet_sk(sk);
		inet->dport = udph->source;
		inet->daddr = iph->saddr;

		if (sk->sk_encap_rcv) {
			if (0 > (res = sk->sk_encap_rcv(sk, skb)))
				return -res;
		}

		assert(udp_prot.backlog_rcv != NULL);
		(*udp_prot.backlog_rcv)(sk, skb);

		if (inet->rcv_saddr == INADDR_ANY) {
			//TODO: temporary
			inet->saddr = skb->nh.iph->daddr;
		}
	} else {
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0);
	}
	return ENOERR;
}

void udp_err(sk_buff_t *skb, uint32_t info) {
	struct sock *sk;
	struct iphdr *emb_pack_iphdr;
	struct udphdr *emb_pack_udphdr;
	int sk_idx = 0;

	/* size_t i; */
	/* __be16 port; */

	emb_pack_iphdr = (struct iphdr *)(skb->h.raw + IP_HEADER_SIZE(skb->nh.iph)
			+ ICMP_HEADER_SIZE);
	emb_pack_udphdr = (struct udphdr *)(skb->h.raw + IP_HEADER_SIZE(skb->nh.iph)
			+ ICMP_HEADER_SIZE + IP_HEADER_SIZE(emb_pack_iphdr));

	/* notify all sockets matching source, dest address, protocol and ports*/
	do {
		sk = _udp_v4_lookup(sk_idx, emb_pack_iphdr->proto, emb_pack_iphdr->saddr,
				emb_pack_iphdr->daddr, emb_pack_udphdr->source, emb_pack_udphdr->dest,
				skb->dev);
		if (sk != NULL) { /* notify socket about an error */
			ip_v4_icmp_err_notify(sk, skb->h.icmph->type, skb->h.icmph->code);
			/* do something else - specific for udp sockets ? */
			sk_idx = _udp_v4_get_hash_idx(sk) + 1;
		}
	} while(sk != NULL);
}

void * get_udp_sockets(void) {
	return (void *)udp_table;
}
