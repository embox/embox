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

#if 0
typedef enum {
	SS_FREE = 0,    /* not allocated */
	SS_UNCONNECTED, /* unconnected to any socket */
	SS_CONNECTING,  /* in process of connecting */
	SS_CONNECTED,   /* connected to socket */
	SS_DISCONNECTING
/* in process of disconnecting  */
} socket_state;
#endif

struct sock;
struct idx_desc;
struct socket_node;
struct family_ops;

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
struct socket {
#if 0
	socket_state state;
#endif
	int type;
	unsigned long flags;

	struct sock *sk;
	struct idx_desc_data *desc_data;
	struct socket_node *socket_node;
	const struct family_ops *ops;
};

struct family_ops {
	int (*release)(struct sock *sk);
	int (*bind)(struct sock *sk, const struct sockaddr *addr,
			socklen_t addrlen);
	int (*connect)(struct sock *sk, const struct sockaddr *addr,
			socklen_t addrlen, int flags);
	int (*listen)(struct sock *sk, int len);
	int (*accept)(struct sock *sk, struct sockaddr *addr,
			socklen_t *addrlen, int flags, struct sock **out_sk);
	int (*sendmsg)(struct sock *sk, struct msghdr *msg,
			int flags);
	int (*recvmsg)(struct sock *sk, struct msghdr *msg,
			int flags);
	int (*shutdown)(struct sock *sk, int how);
	int (*getsockname)(struct sock *sk, struct sockaddr *addr,
			socklen_t *addrlen);
	int (*getpeername)(struct sock *sk, struct sockaddr *addr,
			socklen_t *addrlen);
	int (*getsockopt)(struct sock *sk, int level, int optname,
			void *optval, socklen_t *optlen);
	int (*setsockopt)(struct sock *sk, int level, int optname,
			const void *optval, socklen_t optlen);


	bool  (*compare_addresses)(struct sockaddr *addr1, struct sockaddr *addr2);
};

extern struct socket * socket_alloc(void);
extern void socket_free(struct socket *sock);

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
