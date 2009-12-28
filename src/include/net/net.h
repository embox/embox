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

typedef enum {
        SS_FREE = 0,           /* not allocated                */
        SS_UNCONNECTED,        /* unconnected to any socket    */
        SS_CONNECTING,         /* in process of connecting     */
        SS_CONNECTED,          /* connected to socket          */
        SS_DISCONNECTING       /* in process of disconnecting  */
} socket_state;

enum sock_type {
        SOCK_STREAM     = 1,  /* TCP */
        SOCK_DGRAM      = 2,  /* UDP */
        SOCK_RAW        = 3,  /* IPPROTO_RAW */
};

/**
 * General BSD socket
 * @param state socket state (%SS_CONNECTED, etc)
 * @param type socket type (%SOCK_STREAM, etc)
 * @param flags socket flags (%SOCK_ASYNC_NOSPACE, etc)
 * @param sk internal networking protocol agnostic socket representation
 * @param ops protocol specific socket operations
 */
typedef struct socket {
        socket_state            state;
        short                   type;
        unsigned long           flags;
	//struct file             *file;
        struct sock             *sk;
        const struct proto_ops  *ops;
} socket_t;

struct proto_ops {
        int  family;
        int  (*release)(struct socket *sock);
        int  (*bind)(struct socket *sock, struct sockaddr *myaddr, int sockaddr_len);
        int  (*connect)(struct socket *sock, struct sockaddr *vaddr, int sockaddr_len, int flags);
        int  (*accept)(struct socket *sock, struct socket *newsock, int flags);
        int  (*listen)(struct socket *sock, int len);
        int  (*setsockopt)(socket_t *sock, int level, int optname, char *optval, unsigned int optlen);
        int  (*getsockopt)(socket_t *sock, int level, int optname, char *optval, int *optlen);
        int  (*sendmsg)(/*struct kiocb *iocb,*/ struct socket *sock,/* struct msghdr *m,*/ size_t total_len);
        int  (*recvmsg)(/*struct kiocb *iocb,*/ struct socket *sock,/*struct msghdr *m,*/ size_t total_len, int flags);
};

#endif /* NET_H_ */
