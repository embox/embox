/*
 * ip_v4.c
 *
 *  Created on: 17.03.2009
 *      Author: sunnya
 */
#include "types.h"
#include "net.h"
#include "sock.h"
#include "inet_sock.h"

static inline int rebuild_ip_header_offset( net_packet *pack)
{
	pack->nh.raw = pack->data + (pack->netdev->addr_len * 2 + 2);
	return 0;
}
int ip_received_packet(struct _net_packet *pack)
{
	iphdr *header = pack->nh.iph;
	/*if (ICMP_PROTOCOL_TYPE == header->proto)
	{
		return;
	}*/
	//TODO release ip proto
	return;
}

static int rebuild_ip_header(struct inet_sock *sk, struct _net_packet *pack)
{
	iphdr *hdr = pack->nh.iph;
	hdr->version = 4;
	hdr->ihl = sizeof (iphdr) << 2;
	memcpy(hdr->dst_addr, sk->daddr, sizeof(hdr->dst_addr));
	hdr->len = pack->len;
	hdr->ttl = sk->uc_ttl;
	hdr->frame_id = sk->id;
	hdr->tos = sk->tos;
	//TODO now ip only single frame
	hdr->frame_offset = 0;
	hdr->proto = sk->sk->sk_protocol;

	return 0;
}

int ip_send_packet(struct inet_sock *sk, struct _net_packet *pack)
{
	rebuild_ip_header_offset(pack);
	rebuild_ip_header(sk, pack);
	pack->len += sizeof (iphdr);
	return 0;
}
