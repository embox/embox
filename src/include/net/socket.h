/**
 * @file
 * @brief
 *
 * @date 19.03.09
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef NET_SOCKET_H_
#define NET_SOCKET_H_

#include <linux/aio.h>
#include <netinet/in.h>
#include <net/sock.h>
#include <stdbool.h>
#include <sys/socket.h>

#if 0
#define SYS_SOCKET      1     /* sys_socket(2) */
#define SYS_BIND        2     /* sys_bind(2) */
#define SYS_CONNECT     3     /* sys_connect(2) */
#define SYS_LISTEN      4     /* sys_listen(2) */
#define SYS_ACCEPT      5     /* sys_accept(2) */
#define SYS_GETSOCKNAME 6     /* sys_getsockname(2) */
#define SYS_GETPEERNAME 7     /* sys_getpeername(2) */
#define SYS_SOCKETPAIR  8     /* sys_socketpair(2) */
#define SYS_SEND        9     /* sys_send(2) */
#define SYS_RECV        10    /* sys_recv(2) */
#define SYS_SENDTO      11    /* sys_sendto(2) */
#define SYS_RECVFROM    12    /* sys_recvfrom(2) */
#define SYS_SHUTDOWN    13    /* sys_shutdown(2) */
#define SYS_SETSOCKOPT  14    /* sys_setsockopt(2) */
#define SYS_GETSOCKOPT  15    /* sys_getsockopt(2) */
#define SYS_SENDMSG     16    /* sys_sendmsg(2) */
#define SYS_RECVMSG     17    /* sys_recvmsg(2) */
#endif

typedef enum {
	SS_FREE = 0,    /* not allocated */
	SS_UNCONNECTED, /* unconnected to any socket */
	SS_CONNECTING,  /* in process of connecting */
	SS_CONNECTED,   /* connected to socket */
	SS_DISCONNECTING
/* in process of disconnecting  */
} socket_state;

struct sock;
struct idx_desc;
struct socket_node;
struct proto_ops;
struct kiocb;

/**
 * General BSD socket
 * @param state socket state (%SS_CONNECTED, etc)
 * @param type socket type (%SOCK_STREAM, etc)
 * @param flags socket flags (%SOCK_ASYNC_NOSPACE, etc)
 * @param sk internal networking protocol agnostic socket representation
 * @param desc idx descriptor of a socket. Note: in BSD sockets this field has type 'struct file'
 * @param socket_node node in global socket registry
 * @param ops protocol specific socket operations
 */
typedef struct socket {
	socket_state state;
	short type;
	unsigned long flags;

	struct sock *sk;
	struct idx_desc *desc;
	struct socket_node *socket_node;
	const struct proto_ops *ops;
} socket_t;

struct proto_ops {
	int family;
	int (*release)(struct socket *sock);
	int (*bind)(struct socket *sock, struct sockaddr *myaddr, int sockaddr_len);
	int (*connect)(struct socket *sock, struct sockaddr *vaddr,
			int sockaddr_len, int flags);
	int (*socketpair)(struct socket *sock1, struct socket *sock2);
	/* 1st arg struct sock* - because sockets are handled on kernel sockets level
	   2nd arg struct sock** - not to create excess struct socket, but create it based
	    on alreadey creaed struct sock* */
	int (*accept)(struct sock *sock, struct sock **new_sock, sockaddr_t *addr, int *addr_len);
	int (*getname)(struct socket *sock, struct sockaddr *addr,
			int *sockaddr_len, int peer);

	int (*ioctl)(struct socket *sock, unsigned int cmd, unsigned long arg);
	int (*compat_ioctl)(struct socket *sock, unsigned int cmd,
			unsigned long arg);
	int (*listen)(struct socket *sock, int len);
	int (*shutdown)(struct socket *sock, int flags);
	int (*setsockopt)(struct socket *sock, int level, int optname,
			char *optval, int optlen);
	int (*getsockopt)(struct socket *sock, int level, int optname,
			char *optval, int *optlen);
	int (*compat_setsockopt)(struct socket *sock, int level, int optname,
			char *optval, int optlen);
	int (*compat_getsockopt)(struct socket *sock, int level, int optname,
			char *optval, int *optlen);
	int (*sendmsg)(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
			size_t total_len);
	int (*recvmsg)(struct kiocb *iocb, struct socket *sock, struct msghdr *m,
			size_t total_len, int flags);
	bool  (*compare_addresses)(struct sockaddr *addr1, struct sockaddr *addr2);
};

#if 0
/**
 * close a socket descriptor
 * @param sockfd socket description
 * @return 0 on success. -1 on failure with errno indicating error.
 */
extern int socket_close(int sockfd);
#endif

#if 1 /********** TODO remove this ****************/
#include <net/skbuff.h>
struct sk_buff;
/**
 * Handle encapsulated protocol.
 * @return 0 on success.
 * @return -1 on failure and drop packet.
 */
typedef int (*sk_encap_hnd)(struct sock *sk, struct sk_buff *pack);
extern int check_icmp_err(int sockfd);
extern void socket_set_encap_recv(int sockfd, sk_encap_hnd hnd);
#endif

#endif /* NET_SOCKET_H_ */
