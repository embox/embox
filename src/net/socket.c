/**
 * \file socket.c
 *
 * \date Mar 19, 2009
 * \author anton
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

struct udp_sock* socket_alloc()
{
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

void socket_free(struct udp_sock *sk)
{
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

void fill_sock(struct udp_sock *sk, sk_type type, sk_proto proto)
{
	//TODO:
	sk->inet.sk->sk_protocol = proto;
	sk->inet.sk->sk_type = type;
}

void udpsock_push(net_packet *pack) {
	TRACE("push packet to udp socket\n");
	//TODO:
	int i;
	struct udp_sock *usk;
	udphdr *uh = pack->h.uh;
	iphdr *iph = pack->nh.iph;
	for(i=0; i< MAX_SOCK_NUM; i++) {
		usk = &sks[i].sk;
		if(uh->source == usk->inet.dport &&
		   uh->dest == usk->inet.sport &&
		   iph->saddr == usk->inet.daddr &&
		   iph->daddr == usk->inet.saddr) {
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
	TRACE("create socket\n");
	struct udp_sock *sk;
	sk = socket_alloc();
	fill_sock(sk, type, protocol);
	int i;
        for(i = 0; i < MAX_SOCK_NUM; i++)
        {
                if (sk == &sks[i].sk)
                {
            		sk->inet.id = i;
                        return i;
                }
        }
        return -1;
}

int bind(int s, unsigned char ipaddr[4], int port) {
	TRACE("bind socket\n");
	sks[s].sk.inet.sport = port;
	return -1;
}

void close(int s)
{
	socket_free(&sks[s].sk);
}

int send(int s, const void *buf, int len)
{
	net_packet *pack;
        pack = net_packet_alloc();
	memcpy(pack->data, buf, len);
	//TODO: build pack
        udp_trans(&sks[s].sk, pack);
}

int recv(int s, void *buf, int len)
{
	if(sks[s].new_pack == 1) {
		memcpy(buf, sks[s].queue->data, len);
		net_packet_free(sks[s].queue);
		sks[s].new_pack = 0;
	        return len;
	}
	return 0;
}
