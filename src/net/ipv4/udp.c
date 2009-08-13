/**
 * \file udp.c
 *
 * \date 26.03.2009
 * \author sikmir
 */
#include "common.h"
#include "net/net.h"
#include "net/eth.h"
#include "net/if_device.h"
#include "net/net_pack_manager.h"
#include "net/udp.h"
#include "net/ip.h"
#include "net/socket.h"
#include "net/if_ether.h"

int udp_received_packet(net_packet *pack) {
	LOG_WARN("udp packet received\n");
	return udpsock_push(pack);
}

int udp_init() {
	return 0;
}

static int rebuild_udp_header(net_packet *pack, unsigned short source, unsigned short dest) {
	LOG_DEBUG("rebuild udp header\n");
	udphdr *hdr = pack->h.uh;
	hdr->source = source;
	hdr->dest   = dest;
	hdr->len    = UDP_HEADER_SIZE;
	hdr->check  = Crc16(hdr, UDP_HEADER_SIZE);
	return 0;
}

static void rebuild_udp_packet(net_packet *pack, struct udp_sock *sk, void *ifdev, const void *buf, int len) {
	LOG_DEBUG("rebuild_udp_packet\n");
	if( pack == NULL ||
	    ifdev == NULL ||
	    sk ==NULL) {
		return;
	}
	pack->ifdev = ifdev;
	pack->netdev = ifdev_get_netdevice(ifdev);
	pack->len = UDP_HEADER_SIZE;

	pack->h.raw = pack->data + ETH_HEADER_SIZE + IP_HEADER_SIZE;
	memset(pack->h.raw, 0, UDP_HEADER_SIZE);
	rebuild_udp_header(pack, sk->inet.sport, sk->inet.dport);
	memcpy(pack->h.uh->data, buf, len);
}

int udp_trans(struct udp_sock *sk, void *ifdev, const void *buf, int len) {
	LOG_DEBUG("udp_trans\n");
	net_packet *pack;
        pack = net_packet_alloc();
        if( pack == NULL) {
    		return -1;
        }
	rebuild_udp_packet(pack, sk, ifdev, buf, len);
	return ip_send_packet(&sk->inet, pack);
}
