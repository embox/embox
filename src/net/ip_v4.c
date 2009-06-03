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

static inline int rebuild_ip_header_offset( net_packet *pack) {
	pack->nh.raw = pack->data + MAC_HEADER_SIZE;
	return 0;
}

int ip_received_packet(net_packet *pack) {
//	LOG_DEBUG("ip_received_packet\n");
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

static int rebuild_ip_header(struct inet_sock *sk, struct _net_packet *pack) {
	LOG_DEBUG("rebuild_ip_header\n");
	iphdr *hdr = pack->nh.iph;
	hdr->version = 4;
	hdr->ihl = IP_HEADER_SIZE >> 2;
	memcpy(hdr->saddr, sk->saddr, sizeof(hdr->saddr));
	memcpy(hdr->daddr, sk->daddr, sizeof(hdr->daddr));
	hdr->tot_len = pack->len;
	hdr->ttl = sk->uc_ttl;
	hdr->id = sk->id;
	hdr->tos = sk->tos;
	//TODO now ip only single frame
	hdr->frag_off = 0;
	hdr->proto = UDP_PROTO_TYPE;//sk->sk->sk_protocol;

	return 0;
}

int ip_send_packet(struct inet_sock *sk, struct _net_packet *pack) {
	LOG_DEBUG("ip_send_packet\n");
	rebuild_ip_header_offset(pack);
	rebuild_ip_header(sk, pack);
	pack->len += IP_HEADER_SIZE;
	eth_send(pack);
	return 0;
}
