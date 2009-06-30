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

static int socket_free(struct udp_sock *sk) {
	LOG_DEBUG("socket_free\n");
        int i;
        for(i = 0; i < MAX_SOCK_NUM; i++) {
                if (sk == &sks[i].sk) {
                        sks[i].is_busy = 0;
                        net_packet_free(sks[i].queue);
                        return 0;
                }
        }
        return -1;
}

static void fill_sock(struct udp_sock *sk, int type, int proto) {
	LOG_DEBUG("fill_sock\n");
//	sk->inet.sk->sk_protocol = proto;
//	sk->inet.sk->sk_type = type;
//	sk->inet.sk->netdev = find_net_device("eth0");
	sk->inet.tos = 0;
	sk->inet.uc_ttl = 64;
	sk->inet.id = 0;
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
				sks[i].sk.inet.dport = uh->source;
				memcpy(sks[i].sk.inet.daddr, iph->saddr, sizeof(iph->saddr));
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

int socket(int domain, int type, int protocol) {
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

int bind(int sockfd, const struct sockaddr *addr, int addrlen) {
	LOG_DEBUG("bind socket\n");
        if(sks[sockfd].is_busy == 0) {
    		return -1;
        }
	memcpy(&sks[sockfd].sk.inet.saddr, &(addr->sa_data[2]), IP_ADDR_LEN);
	memcpy(&sks[sockfd].sk.inet.sport, &(addr->sa_data[0]), sizeof(short));
	char ip[15];
	ipaddr_print(ip, sks[sockfd].sk.inet.saddr);
	LOG_DEBUG("socket binded at port=%d, ip=%s\n", sks[sockfd].sk.inet.sport, ip);
	return 0;
}

int close(int sockfd) {
	LOG_DEBUG("close\n");
	return socket_free(&sks[sockfd].sk);
}

int send(int sockfd, const void *buf, int len, int flags) {
	LOG_DEBUG("send\n");
	if(sks[sockfd].is_busy == 0) {
	        return -1;
	}
        udp_trans(&sks[sockfd].sk, sks[sockfd].queue->ifdev, buf, len);
	return len;
}

int recv(int sockfd, void *buf, int len, int flags) {
	LOG_DEBUG("recv\n");
	if(sks[sockfd].is_busy == 0) {
		return -1;
	}
	while(1) {
		if(sks[sockfd].new_pack == 1) {
			LOG_DEBUG("received packet\n");
			memcpy(buf, sks[sockfd].queue->data + MAC_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE - 24, len);
			net_packet_free(sks[sockfd].queue);
			sks[sockfd].new_pack = 0;
	    		return len;
		}
	}
}
