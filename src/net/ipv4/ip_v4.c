/**
 * \file ip_v4.c
 *
 * \date 17.03.2009
 * \author sunnya
 */
#include "types.h"
#include "common.h"
#include "net.h"
#include "sock.h"
#include "inet_sock.h"
#include "mac.h"

int ip_received_packet(net_packet *pack) {
	iphdr *header = pack->nh.iph;
	if (ICMP_PROTO_TYPE == header->proto) {
                icmp_received_packet(pack);
        }
        if (UDP_PROTO_TYPE == header->proto) {
                packet_dump(pack);
                udp_received_packet(pack);
        }
	return 0;
}

/**
 * Fill IP header
 */
int rebuild_ip_header(net_packet *pack, unsigned char ttl, unsigned char proto,
			unsigned short id, unsigned short len, unsigned char saddr[4], unsigned char daddr[4]) {
	iphdr *hdr    = pack->nh.iph;
	hdr->version  = 4;
	hdr->ihl      = IP_HEADER_SIZE >> 2;
	memcpy(hdr->saddr, saddr, sizeof(hdr->saddr));
	memcpy(hdr->daddr, daddr, sizeof(hdr->daddr));
	hdr->tot_len  = len;
        hdr->ttl      = ttl;
        hdr->id       = id;
        hdr->tos      = 0;
        hdr->frag_off = 0;
        hdr->proto    = proto;
        hdr->check    = calc_checksumm(pack->nh.raw, IP_HEADER_SIZE);
	return 0;
}

static int build_ip_packet(struct inet_sock *sk, net_packet *pack) {
	pack->nh.raw = pack->data + MAC_HEADER_SIZE;
	rebuild_ip_header(pack, sk->uc_ttl, sk->sk->sk_protocol,
			  sk->id, pack->len, sk->saddr, sk->daddr);
	return 0;
}

int ip_send_packet(struct inet_sock *sk, net_packet *pack) {
	LOG_DEBUG("ip_send_packet\n");
	build_ip_packet(sk, pack);
	pack->protocol = IP_PROTOCOL_TYPE;
	pack->len += IP_HEADER_SIZE;
	return eth_send(pack);
}
