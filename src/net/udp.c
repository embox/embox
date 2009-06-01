/**
 * \file udp.c
 *
 * \date 26.03.2009
 * \author sikmir
 */
#include "types.h"
#include "common.h"
#include "net.h"
#include "eth.h"
#include "net_pack_manager.h"
#include "udp.h"
#include "ip_v4.h"
#include "socket.h"

int udp_received_packet(net_packet *pack) {
	LOG_DEBUG("udp packet received\n");
	udpsock_push(pack);
	return 0;
}

int udp_init(void) {
	//TODO:
	return 0;
}

void rebuild_udp_packet(net_packet *pack, struct udp_sock *sk, void *ifdev, const void *buf, int len) {
	LOG_DEBUG("rebuild_udp_packet\n");
	if( pack == NULL ||
	    ifdev == NULL ||
	    sk ==NULL) {
		return;
	}
	pack->ifdev = ifdev;
	pack->netdev = eth_get_netdevice(ifdev);
	pack->protocol = UDP_PROTO_TYPE;
	pack->len = UDP_HEADER_SIZE;
	pack->h.raw = pack->nh.raw + MAC_HEADER_SIZE + IP_HEADER_SIZE;
	memset(pack->h.raw, 0, UDP_HEADER_SIZE);
	pack->h.uh->source = sk->inet.sport;
	pack->h.uh->dest = sk->inet.dport;
	pack->h.uh->len = UDP_HEADER_SIZE;
	pack->h.uh->check = 0;
	memcpy(pack->h.uh->data, buf, len);
}

int udp_trans(struct udp_sock *sk, void *ifdev, const void *buf, int len) {
	LOG_DEBUG("udp_trans\n");
	net_packet *pack;
        pack = net_packet_alloc();
	rebuild_udp_packet(pack, sk, ifdev, buf, len);
	ip_send_packet(sk, pack);
	return 0;
}
