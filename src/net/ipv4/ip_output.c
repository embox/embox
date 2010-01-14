/**
 * @file ip_output.c
 *
 * @brief The Internet Protocol (IP) output module.
 * @date 03.12.2009
 * @author Nikolay Korotky
 */
#include "err.h"
#include "kernel/module.h"
#include "net/net.h"
#include "net/skbuff.h"
#include "net/ip.h"
#include "net/inet_sock.h"
#include "net/if_ether.h"
#include "net/netdevice.h"
#include "net/inetdevice.h"
#include "net/route.h"
#include "net/checksum.h"

void __init ip_init(void) {
        ip_rt_init();
}

/* Generate a checksum for an outgoing IP datagram. */
inline void ip_send_check(iphdr_t *iph) {
        iph->check = 0;
        iph->check = ptclbsum((void*)iph, IP_HEADER_SIZE);
}

int rebuild_ip_header(sk_buff_t *pack, unsigned char ttl, unsigned char proto,
		unsigned short id, unsigned short len, in_addr_t saddr,
		in_addr_t daddr) {
	iphdr_t *hdr = pack->nh.iph;
	hdr->version = 4;
	hdr->ihl = IP_HEADER_SIZE >> 2;
	hdr->saddr = saddr;
	hdr->daddr = daddr;
	hdr->tot_len = len;
	hdr->ttl = ttl;
	hdr->id = id;
	hdr->tos = 0;
	hdr->frag_off = IP_DF;
	hdr->proto = proto;
	ip_send_check(hdr);
	return 0;
}

static int build_ip_packet(struct inet_sock *sk, sk_buff_t *pack) {
	pack->nh.raw = pack->data + ETH_HEADER_SIZE;
	rebuild_ip_header(pack, sk->uc_ttl, sk->sk.sk_protocol, sk->id, pack->len,
			sk->saddr, sk->daddr);
	return 0;
}

int ip_queue_xmit(sk_buff_t *skb) {
	//TODO:
	skb->nh.iph->ttl      = 64;
	return dev_queue_xmit(skb);
}

int ip_send_packet(struct inet_sock *sk, sk_buff_t *pack) {
	build_ip_packet(sk, pack);
	pack->protocol = ETH_P_IP;
	pack->len += IP_HEADER_SIZE;
	ip_route(pack);
	return ip_queue_xmit(pack);
}

void ip_send_reply(struct sock *sk, in_addr_t saddr, in_addr_t daddr,
			    sk_buff_t *pack, unsigned int len) {
	pack->nh.iph->saddr = saddr;
        pack->nh.iph->daddr = daddr;
        pack->nh.iph->id ++;
        pack->nh.iph->frag_off = IP_DF;
        ip_send_check(pack->nh.iph);
	ip_queue_xmit(pack);
}
