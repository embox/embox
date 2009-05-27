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

void build_udp_packet(net_packet *pack, net_device *netdev, void *ifdev, const void *buf, int len) {
        pack->netdev = netdev;
        pack->ifdev = ifdev;
        //TODO: build udp header
//        pack->nh.mac = pack->data;
//        pack->nh.raw = pack->data + sizeof(machdr);
//        pack->h.raw = pack->nh.raw + sizeof(iphdr);
//        buff = pack->h.raw + sizeof(udp);
}

int udp_trans(struct inet_sock *sk, net_packet *pack) {
	LOG_DEBUG("udp_trans\n");
	//ip_send_packet(sk, pack);
	eth_send(pack);
	return 0;
}
