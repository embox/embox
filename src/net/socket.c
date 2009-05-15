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
	DEBUG("socket_alloc");
        int i;
        for(i = 0; i < MAX_SOCK_NUM; i++)
        {
                if (0 == sks[i].is_busy)
                {
                        sks[i].is_busy = 1;
                        sks[i].new_pack = 0;
                        memset(&sks[i].sk, 0, sizeof(sks[i].sk));
                        return &sks[i].sk;
                }
        }
        return NULL;
}

void socket_free(struct udp_sock *sk) {
	DEBUG("socket_free");
        int i;
        for(i = 0; i < MAX_SOCK_NUM; i++)
        {
                if (sk == &sks[i].sk)
                {
                        sks[i].is_busy = 0;
                        net_packet_free(sks[i].queue);
                }
        }
}

void fill_sock(struct udp_sock *sk, sk_type type, sk_proto proto) {
	DEBUG("fill_sock");
	//TODO:
//	sk->inet.sk->sk_protocol = proto;
//	sk->inet.sk->sk_type = type;
}

void udpsock_push(net_packet *pack) {
	DEBUG("push packet to udp socket");
	int i;
	struct udp_sock *usk;
	udphdr *uh = pack->h.uh;
	iphdr *iph = pack->nh.iph;
	for(i=0; i< MAX_SOCK_NUM; i++) {
		usk = &sks[i].sk;
		if(uh->dest == usk->inet.sport &&
		   (0 == memcmp(iph->daddr, usk->inet.saddr, 4))) {
			if(sks[i].new_pack == 0) {
			    sks[i].queue = pack;
			    sks[i].new_pack = 1;
			}
			return;
		}
	}
	//TODO: send icmp unsuccess response
	net_packet_free(pack);
}

int socket(sk_type type, sk_proto protocol) {
	DEBUG("create socket");
	struct udp_sock *sk;
	if((sk = socket_alloc()) == NULL) {
		ERROR("Can't alloc socket.");
		return -1;
	}
	fill_sock(sk, type, protocol);
	int i;
        for(i = 0; i < MAX_SOCK_NUM; i++)
        {
                if (sk == &sks[i].sk)
                {
            		DEBUG("socket id=%d", i);
                        return i;
                }
        }
        return -1;
}

int bind(int s, unsigned char ipaddr[4], int port) {
	DEBUG("bind socket");
	memcpy(&sks[s].sk.inet.saddr, ipaddr, sizeof(ipaddr));
	sks[s].sk.inet.sport = port;
	DEBUG("socket binded at port=%d, ip=", sks[s].sk.inet.sport); ipaddr_print(sks[s].sk.inet.saddr, 4);
	return 0;
}

void close(int s) {
	DEBUG("close");
	socket_free(&sks[s].sk);
}

int send(int s, const void *buf, int len) {
	DEBUG("send");
	net_packet *pack;
        pack = net_packet_alloc();
	memcpy(pack->data, buf, len);
	//TODO: build pack
        udp_trans(&sks[s].sk, pack);
}

int recv(int s, void *buf, int len) {
	if(sks[s].new_pack == 1) {
		DEBUG("recv");
		memcpy(buf, sks[s].queue->h.uh->data, len);
		net_packet_free(sks[s].queue);
		sks[s].new_pack = 0;
	        return len;
	}
	return 0;
}
