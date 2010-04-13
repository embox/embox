/**
 * @file
 * @brief The Internet Protocol (IP) output module.
 *
 * @date 03.12.2009
 * @author Nikolay Korotky
 */

#include <err.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/inet_sock.h>
#include <net/inetdevice.h>
#include <net/route.h>
#include <net/checksum.h>
#include <linux/init.h>

void __init ip_init(void) {
	ip_rt_init();
}

/* Generate a checksum for an outgoing IP datagram. */
inline static void ip_send_check(iphdr_t *iph) {
	iph->check = 0;
	iph->check = ptclbsum((void*)iph, IP_HEADER_SIZE);
}

int rebuild_ip_header(sk_buff_t *skb, unsigned char ttl, unsigned char proto,
		unsigned short id, unsigned short len, in_addr_t saddr,
		in_addr_t daddr) {
	iphdr_t *hdr = skb->nh.iph;
	hdr->version = 4;
	hdr->ihl = IP_HEADER_SIZE >> 2;
	hdr->saddr = saddr;
	hdr->daddr = daddr;
	hdr->tot_len = len - ETH_HEADER_SIZE;
	hdr->ttl = ttl;
	hdr->id = id;
	hdr->tos = 0;
	hdr->frag_off = IP_DF;
	hdr->proto = proto;
	ip_send_check(hdr);
	return 0;
}

static int build_ip_packet(struct inet_sock *sk, sk_buff_t *skb) {
	skb->nh.raw = skb->data + ETH_HEADER_SIZE;
	rebuild_ip_header(skb, sk->uc_ttl, sk->sk.sk_protocol, sk->id, skb->len,
			sk->saddr, sk->daddr);
	return 0;
}

int ip_queue_xmit(sk_buff_t *skb, int ipfragok) {
//	struct sock *sk = skb->sk;
//	struct inet_sock *inet = inet_sk(sk);
//	struct ip_options *opt = inet->opt;
//	struct iphdr *iph = skb->nh.iph;
	/*TODO: route*/
//	iph->ttl      = 64;
	skb->protocol = ETH_P_IP;
	return dev_queue_xmit(skb);
}

int ip_send_packet(struct inet_sock *sk, sk_buff_t *skb) {
	skb->nh.raw = (unsigned char *) skb->data + ETH_HEADER_SIZE;
	if(sk->sk.sk_type != SOCK_RAW) {
		build_ip_packet(sk, skb);
		//skb->len += IP_HEADER_SIZE;
	}
	ip_route(skb);
	ip_send_check(skb->nh.iph);
	return ip_queue_xmit(skb, 0);
}

void ip_send_reply(struct sock *sk, in_addr_t saddr, in_addr_t daddr,
			sk_buff_t *skb, unsigned int len) {
	skb->nh.iph->saddr = saddr;
	skb->nh.iph->daddr = daddr;
	skb->nh.iph->id ++;
	skb->nh.iph->frag_off = IP_DF;
	ip_send_check(skb->nh.iph);
	ip_queue_xmit(skb, 0);
}
