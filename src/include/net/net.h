/**
 * @file net.h
 *
 * @date 11.03.2009
 * @author Anton Bondarev
 */
#ifndef NET_H_
#define NET_H_

#include <net/socket.h>

#define NET_INTERFACES_QUANTITY     0x4
#define IPV4_ADDR_LENGTH            0x4

typedef unsigned char enet_addr_t[6];

enum sock_type {
        SOCK_STREAM     = 1,  /* TCP */
        SOCK_DGRAM      = 2,  /* UDP */
        SOCK_RAW        = 3,  /* IPPROTO_RAW */
};

struct socket {
        //socket_state            state;
        short                   type;
        unsigned long           flags;
	//struct file             *file;
        struct sock             *sk;
        const struct proto_ops  *ops;
};

struct proto_ops {
        int  family;
        int  (*release)(struct socket *sock);
        int  (*bind)(struct socket *sock, struct sockaddr *myaddr, int sockaddr_len);
        int  (*connect)(struct socket *sock, struct sockaddr *vaddr, int sockaddr_len, int flags);
        int  (*accept)(struct socket *sock, struct socket *newsock, int flags);
        int  (*listen)(struct socket *sock, int len);
        int  (*sendmsg)(/*struct kiocb *iocb,*/ struct socket *sock,/* struct msghdr *m,*/ size_t total_len);
        int  (*recvmsg)(/*struct kiocb *iocb,*/ struct socket *sock,/*struct msghdr *m,*/ size_t total_len, int flags);
};

#endif /* NET_H_ */
