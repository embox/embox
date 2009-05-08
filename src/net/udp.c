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
	TRACE("udp packet received\n");
	udpsock_push(pack);
	return 0;
}

int udp_init(void) {
	//TODO:
	return 0;
}

int udp_trans(struct inet_sock *sk, net_packet *pack) {
	//TODO: build udp header
//        pack->nh.mac = pack->data;
//        pack->nh.raw = pack->data + sizeof(machdr);
//        pack->h.raw = pack->nh.raw + sizeof(iphdr);
//        buff = pack->h.raw + sizeof(udp);
	ip_send_packet(sk, pack);
	return 0;
}
