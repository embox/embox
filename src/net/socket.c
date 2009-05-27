/**
 * \file socket.c
 *
 * \date Mar 19, 2009
 * \author anton, sikmir
 */

#include "types.h"
#include "common.h"
#include "socket.h"
#include "udp.h"
#include "net_pack_manager.h"

typedef struct _SOCK_INFO{
        struct udp_sock sk;
        net_packet *queue; //stub
        int new_pack;
        int is_busy;
}SOCK_INFO;

static SOCK_INFO sks[MAX_SOCK_NUM];

struct udp_sock* socket_alloc() {
	LOG_DEBUG("socket_alloc\n");
        int i;
        for(i = 0; i < MAX_SOCK_NUM; i++) {
                if (0 == sks[i].is_busy) {
                        sks[i].is_busy = 1;
                        sks[i].new_pack = 0;
                        memset(&sks[i].sk, 0, sizeof(sks[i].sk));
                        return &sks[i].sk;
                }
        }
        return NULL;
}

void socket_free(struct udp_sock *sk) {
	LOG_DEBUG("socket_free\n");
        int i;
        for(i = 0; i < MAX_SOCK_NUM; i++) {
                if (sk == &sks[i].sk) {
                        sks[i].is_busy = 0;
                        net_packet_free(sks[i].queue);
                }
        }
}

void fill_sock(struct udp_sock *sk, sk_type type, sk_proto proto) {
	LOG_DEBUG("fill_sock\n");
	//TODO:
//	sk->inet.sk->sk_protocol = proto;
//	sk->inet.sk->sk_type = type;
}

int udpsock_push(net_packet *pack) {
	LOG_DEBUG("push packet to udp socket\n");
	int i;
	struct udp_sock *usk;
	udphdr *uh = pack->h.uh;
	iphdr *iph = pack->nh.iph;
	for(i=0; i< MAX_SOCK_NUM; i++) {
		usk = &sks[i].sk;
		if(uh->dest == usk->inet.sport &&
		   (0 == memcmp(iph->daddr, usk->inet.saddr, 4))) {
			if(sks[i].new_pack == 0) {
			    LOG_DEBUG("packet pushed\n");
			    sks[i].queue = net_packet_copy(pack);
			    sks[i].new_pack = 1;
			    return 0;
			} else {
			    LOG_DEBUG("queue is busy\n");
			    return -1;
			}
		}
	}
	LOG_DEBUG("destination socket not found\n");
	//TODO: send icmp unsuccess response
	return -1;
}

int socket(sk_type type, sk_proto protocol) {
	LOG_DEBUG("create socket\n");
	struct udp_sock *sk;
	if((sk = socket_alloc()) == NULL) {
		LOG_ERROR("Can't alloc socket.\n");
		return -1;
	}
	fill_sock(sk, type, protocol);
	int i;
        for(i = 0; i < MAX_SOCK_NUM; i++) {
                if (sk == &sks[i].sk) {
            		LOG_DEBUG("socket id=%d\n", i);
                        return i;
                }
        }
        return -1;
}

int bind(int s, unsigned char ipaddr[4], int port) {
	LOG_DEBUG("bind socket\n");
	memcpy(&sks[s].sk.inet.saddr, ipaddr, sizeof(ipaddr));
	sks[s].sk.inet.sport = port;
	LOG_DEBUG("socket binded at port=%d, ip=", sks[s].sk.inet.sport); ipaddr_print(sks[s].sk.inet.saddr, 4);
	LOG_DEBUG("\n");
	return 0;
}

void close(int s) {
	LOG_DEBUG("close\n");
	socket_free(&sks[s].sk);
}

int send(int s, const void *buf, int len) {
	LOG_DEBUG("send\n");
	net_packet *pack;
        pack = net_packet_alloc();
	build_udp_packet(pack, sks[s].queue->netdev, sks[s].queue->ifdev, buf, len);
        udp_trans(&sks[s].sk.inet, pack);
}

int recv(int s, void *buf, int len) {
	if(sks[s].new_pack == 1) {
		LOG_DEBUG("recv\n");
		memcpy(buf, sks[s].queue->data + sizeof(machdr) + sizeof(iphdr) + sizeof(udphdr), len);
		net_packet_free(sks[s].queue);
		sks[s].new_pack = 0;
	        return len;
	}
	return 0;
}
