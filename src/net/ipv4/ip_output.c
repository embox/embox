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
#include "lib/inet/netinet/in.h"
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
	hdr->check = 0;
	hdr->check = ptclbsum(pack->nh.raw, IP_HEADER_SIZE);
	return 0;
}

static int build_ip_packet(struct inet_sock *sk, sk_buff_t *pack) {
	pack->nh.raw = pack->data + ETH_HEADER_SIZE;
	rebuild_ip_header(pack, sk->uc_ttl, sk->sk.sk_protocol, sk->id, pack->len,
			sk->saddr, sk->daddr);
	return 0;
}

int ip_queue_xmit(struct sk_buff *skb) {
	//TODO:
	dev_queue_xmit(skb);
}

int ip_send_packet(struct inet_sock *sk, sk_buff_t *pack) {
	build_ip_packet(sk, pack);
	pack->protocol = ETH_P_IP;
	pack->len += IP_HEADER_SIZE;
	ip_route(pack);
	return ip_queue_xmit(pack);
}
