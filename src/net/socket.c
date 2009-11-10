/**
 * \file socket.c
 *
 * \date Mar 19, 2009
 * \author anton, sikmir
 */

#include "conio.h"
#include "string.h"
#include "common.h"
#include "net/skbuff.h"
#include "net/socket.h"
#include "net/udp.h"
#include "net/ip.h"
#include "net/if_ether.h"
#include "net/net_pack_manager.h"
#include "net/net_packet.h"

void fill_sock(struct udp_sock *sk, int type, int proto) {
	LOG_DEBUG("fill_sock\n");
	sk->inet.sk.sk_protocol = proto;
	sk->inet.sk.sk_type = type;
//	sk->inet.sk->netdev = netdev_get_by_name("eth0");
	sk->inet.tos = 0;
	sk->inet.uc_ttl = 64;
	sk->inet.id = 0;
}

int socket(int domain, int type, int protocol) {
	LOG_DEBUG("create socket\n");
	struct udp_sock *sk;
	if((sk = (struct udp_sock *)sk_alloc()) == NULL) {
		LOG_ERROR("Can't alloc socket.\n");
		return -1;
	}
	fill_sock(sk, type, UDP_PROTO_TYPE);
	int i;
        for(i = 0; i < MAX_SOCK_NUM; i++) {
                if (sk == sks[i].sk) {
            		LOG_DEBUG("socket id=%d\n", i);
                        return i;
                }
        }
        return -1;
}

int connect (int sockfd, const struct sockaddr *addr, int addrlen) {
	LOG_DEBUG("connect socket\n");
	if(sks[sockfd].is_busy == 0) {
	    return -1;
	}
	memcpy(&sks[sockfd].sk->inet.daddr, &(addr->sa_data[2]), IP_ADDR_LEN);
	memcpy(&sks[sockfd].sk->inet.dport, &(addr->sa_data[0]), sizeof(short));
	char ip[15];
	ipaddr_print(ip, sks[sockfd].sk->inet.daddr);
	//TODO what is it
	sks[sockfd].queue = alloc_skb (0x100, 0);
	sks[sockfd].queue->ifdev = (void*)inet_dev_find_by_name ("eth0");
	LOG_WARN("socket connected at port=%d, ip=%s ifdev = %d\n", sks[sockfd].sk->inet.sport, ip, sks[sockfd].queue);
	return 0;
}

int bind(int sockfd, const struct sockaddr *addr, int addrlen) {
	LOG_DEBUG("bind socket\n");
        if(sks[sockfd].is_busy == 0) {
    		return -1;
        }
	memcpy(&sks[sockfd].sk->inet.saddr, &(addr->sa_data[2]), IP_ADDR_LEN);
	memcpy(&sks[sockfd].sk->inet.sport, &(addr->sa_data[0]), sizeof(short));
	char ip[15];
	ipaddr_print(ip, sks[sockfd].sk->inet.saddr);
	LOG_DEBUG("socket binded at port=%d, ip=%s\n", sks[sockfd].sk->inet.sport, ip);
	return 0;
}

int close(int sockfd) {
	LOG_DEBUG("close\n");
	sk_free(sks[sockfd].sk);
	return 0;
}

int send(int sockfd, const void *buf, int len, int flags) {
	LOG_DEBUG("send\n");
	if(sks[sockfd].is_busy == 0) {
	        return -1;
	}
        udp_trans(sks[sockfd].sk, sks[sockfd].queue->ifdev, buf, len);
	return len;
}

int recv(int sockfd, void *buf, int len, int flags) {
	LOG_DEBUG("recv\n");
	if(sks[sockfd].is_busy == 0) {
		return -1;
	}
	while(sks[sockfd].is_busy == 1) {
		if(sks[sockfd].new_pack == 1) {
			LOG_DEBUG("received packet\n");
			memcpy(buf, sks[sockfd].queue->data + ETH_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE - 24, len);
			kfree_skb(sks[sockfd].queue);
			sks[sockfd].new_pack = 0;
	    		return len;
		}
	}
	return -1;
}

int empty_socket(int sockfd) {
	LOG_DEBUG("recv\n");
        if(sks[sockfd].is_busy == 0) {
                return -1;
        }
	return (1 - sks[sockfd].new_pack);
}

