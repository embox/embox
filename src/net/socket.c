/**
 * @file socket.c
 *
 * @brief An implementation of the SOCKET network access protocol.
 * @date 19.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 */

#include <stdio.h>
#include <string.h>
#include <common.h>
#include <kernel/module.h>
#include <net/skbuff.h>
#include <net/socket.h>
#include <net/udp.h>
#include <net/ip.h>
#include <net/if_ether.h>
#include <net/net_pack_manager.h>

/**
 * Allocate a new socket object.
 */
static struct socket *sock_alloc() {
	struct socket *sock;
	//TODO:
	return sock;
}

/**
 * Close a socket
 */
void sock_release(struct socket *sock) {
	if (sock->ops) {
		sock->ops->release(sock);
		sock->ops = NULL;
	}
}

int __init sock_init(void) {
        /* Initialize sock. */
        //sk_init();

        /* Initialize skbuff. */
        skb_init();
        return 0;
}

#if 0
void fill_sock(struct udp_sock *sk, int type, int proto) {
        sk->inet.sk.sk_protocol = proto;
        sk->inet.sk.sk_type = type;
//	sk->inet.sk->netdev = netdev_get_by_name("eth0");
        sk->inet.tos = 0;
        sk->inet.uc_ttl = 64;
        sk->inet.id = 0;
}

int socket(int domain, int type, int protocol) {
        struct udp_sock *sk;
        if((sk = (struct udp_sock *)sk_alloc()) == NULL) {
                LOG_ERROR("Can't alloc socket.\n");
                return -1;
        }
        fill_sock(sk, type, UDP_PROTO_TYPE);
        int i;
        for(i = 0; i < MAX_SOCK_NUM; i++) {
                if (sk == sks[i].sk) {
                        return i;
                }
        }
        return -1;
}

int connect (int sockfd, const struct sockaddr *addr, int addrlen) {
        if(sks[sockfd].is_busy == 0) {
                return -1;
        }
        memcpy(&sks[sockfd].sk->inet.daddr, &(addr->sa_data[2]), IP_ADDR_LEN);
        memcpy(&sks[sockfd].sk->inet.dport, &(addr->sa_data[0]), sizeof(short));
        //TODO what is it
        sks[sockfd].queue = alloc_skb (0x100, 0);
        //TODO we haven't ifdev now
#if 0
        sks[sockfd].queue->ifdev = (void*)inet_dev_find_by_name ("eth0");
#endif
        struct in_addr ip;
        ip.s_addr = sks[sockfd].sk->inet.daddr;
        LOG_WARN("socket connected at port=%d, ip=%s ifdev = %d\n", sks[sockfd].sk->inet.sport,
                 inet_ntoa(ip), sks[sockfd].queue);
        return 0;
}

int bind(int sockfd, const struct sockaddr *addr, int addrlen) {
        if(sks[sockfd].is_busy == 0) {
                return -1;
        }
        memcpy(&sks[sockfd].sk->inet.saddr, &(addr->sa_data[2]), IP_ADDR_LEN);
        memcpy(&sks[sockfd].sk->inet.sport, &(addr->sa_data[0]), sizeof(short));
#if 0
        struct in_addr ip;
        ip.s_addr = sks[sockfd].sk->inet.saddr;
#endif
        return 0;
}

int close(int sockfd) {
        sk_free(sks[sockfd].sk);
        return 0;
}

int send(int sockfd, const void *buf, int len, int flags) {
        if(sks[sockfd].is_busy == 0) {
                return -1;
        }
        //TODO we haven't ifdev now
#if 0
        udp_trans(sks[sockfd].sk, sks[sockfd].queue->ifdev, buf, len);
#endif
        return len;
}

int recv(int sockfd, void *buf, int len, int flags) {
        if(sks[sockfd].is_busy == 0) {
                return -1;
        }
        while(sks[sockfd].is_busy == 1) {
                if(sks[sockfd].new_pack == 1) {
                        memcpy(buf, sks[sockfd].queue->data + ETH_HEADER_SIZE + IP_HEADER_SIZE + UDP_HEADER_SIZE - 24, len);
                        kfree_skb(sks[sockfd].queue);
                        sks[sockfd].new_pack = 0;
                        return len;
                }
        }
        return -1;
}

int empty_socket(int sockfd) {
        if(sks[sockfd].is_busy == 0) {
                return -1;
        }
        return (1 - sks[sockfd].new_pack);
}
#endif
