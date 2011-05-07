/**
 * @file
 * @brief Definitions for the AF_INET socket handler.
 *
 * @date 17.03.09
 * @author Anton Bondarev
 */

#ifndef SOCK_H_
#define SOCK_H_

#include <net/netdevice.h>
#include <net/net.h>

typedef struct {
	spinlock_t slock;
} socket_lock_t;

/**
 * Minimal network layer representation of sockets.
 * @param skc_family network address family
 * @param skc_state Connection state
 * @param skc_reuse %SO_REUSEADDR setting
 * @param skc_bound_dev_if bound device index if != 0
 */
struct sock_common {
	unsigned short skc_family;
	volatile unsigned char skc_state;
	unsigned char skc_reuse;
	int skc_bound_dev_if;
#if 0
	struct hlist_node skc_node;
	struct hlist_node skc_bind_node;
	atomic_t skc_refcnt;
	unsigned int skc_hash;
#endif
	struct proto *skc_prot;
};

/**
 * Network layer representation of sockets.
 * @param __sk_common shared layout with inet_timewait_sock
 * @param sk_protocol which protocol this socket belongs in this network family
 * @param sk_type socket type
 * @param sk_rcvbuf size of receive buffer in bytes
 * @param sk_lock synchronizer
 * @param sk_sndbuf size of send buffer in bytes
 * @param sk_flags %SO_LINGER (l_onoff), %SO_BROADCAST, %SO_KEEPALIVE,
 *			%SO_OOBINLINE settings, %SO_TIMESTAMPING settings
 * @param sk_receive_queue incoming packets
 * @param sk_write_queue Packet sending queue
 * @param sk_socket Identd and reporting IO signals
 * @param sk_state_change: callback to indicate change in the state of the sock
 * @param sk_data_ready: callback to indicate there is data to be processed
 * @param sk_write_space: callback to indicate there is bf sending space available
 * @param sk_error_report: callback to indicate errors (e.g. %MSG_ERRQUEUE)
 * @param sk_backlog_rcv: callback to process the backlog
 * @param sk_destruct: called at sock freeing time, i.e. when all refcnt == 0
 */
typedef struct sock {
	struct sock_common __sk_common;
#define sk_family  __sk_common.skc_family
#define sk_prot    __sk_common.skc_prot
	unsigned char sk_protocol;
	unsigned short sk_type;
	int sk_rcvbuf;
	socket_lock_t sk_lock;
	struct {
		struct sk_buff *head;
		struct sk_buff *tail;
	} sk_backlog;
	int sk_sndbuf;
	unsigned long sk_flags;

	struct sk_buff_head *sk_receive_queue;
	struct sk_buff_head *sk_write_queue;

	struct socket *sk_socket;
	void *sk_user_data;

	void (* sk_state_change)(struct sock *sk);
	void (* sk_data_ready)(struct sock *sk, int bytes);
	void (* sk_write_space)(struct sock *sk);
	void (* sk_error_report)(struct sock *sk);
	int (* sk_backlog_rcv)(struct sock *sk, sk_buff_t *pack);
	void (* sk_destruct)(struct sock *sk);
} sock_t;

/** Sock flags */
enum sock_flags {
	SOCK_DEAD,
	SOCK_DONE,
	SOCK_DESTROY,
	SOCK_BROADCAST
};

/** Protocol specific functions */
typedef struct proto {
	void (*close)(sock_t *sk, long timeout);
	int (*connect)(sock_t *sk, sockaddr_t *uaddr, int addr_len);
	int (*disconnect)(sock_t *sk, int flags);
	sock_t *(*accept)(sock_t *sk, int flags, int *err);
	int (*ioctl)(struct sock *sk, int cmd, unsigned long arg);
	int (*init)(sock_t *sk);
	void (*destroy)(struct sock *sk);
	int (*setsockopt)(struct sock *sk, int level, int optname, char *optval,
			int optlen);
	int (*getsockopt)(struct sock *sk, int level, int optname, char *optval,
			int *option);
	int (*sendmsg)(struct kiocb *iocb, sock_t *sk, struct msghdr *msg,
			size_t len);
	int (*recvmsg)(struct kiocb *iocb, sock_t *sk, struct msghdr *msg,
			size_t len, int noblock, int flags, int *addr_len);
	int (*bind)(sock_t *sk, sockaddr_t *uaddr, int addr_len);
	int (*backlog_rcv)(sock_t *sk, sk_buff_t *skb);
	void (*hash)(struct sock *sk);
	void (*unhash)(struct sock *sk);
	unsigned int obj_size;
	char name[32];
} proto_t;

/** Initializes structure sock's pull */
extern void sk_init(void);

/** Allocates structure sock with specific parameters
 * @family - Protocol family (PF_INIT for example)
 * @priority -not use now
 * @prot - pointer to the proto structure
 */
extern sock_t *sk_alloc(/*struct net *net,*/int family, gfp_t priority,
		proto_t *prot);

/**
 * Returns specified structure sock into pull,
 * assuming there are no more handle on it.
 */
extern void sk_free(sock_t *sk);

/** This function used by all transports to attempt to queue received packets*/
extern int sock_queue_rcv_skb(sock_t *sk, sk_buff_t *skb);

/**
 * Functions to fill in entries in struct proto_ops when a protocol
 * does not implement a particular function.
 */
extern int sock_no_listen(struct socket *, int);
extern int sock_no_accept(struct socket *, struct socket *, int);

extern int sock_common_recvmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *msg, size_t size, int flags);

extern void sk_common_release(struct sock *sk);

#if 0
//TODO NETSOCK: functions are not realized now
extern int proto_register(proto_t *prot, int alloc_slab);
extern void proto_unregister(proto_t *prot);

extern sock_t *sk_clone(const sock_t *sk, const gfp_t priority);
extern int sock_setsockopt(socket_t *sock, int level, int op, char *optval, unsigned int optlen);
extern int sock_getsockopt(socket_t *sock, int level, int op, char *optval, int *optlen);
extern sk_buff_t *sock_alloc_send_skb(sock_t *sk, unsigned long size, int noblock, int *errcode);
#endif

#endif /* SOCK_H_ */
