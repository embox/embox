/**
 * @file
 * @brief
 *
 * @date 17.03.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef NET_SOCK_H_
#define NET_SOCK_H_

#include <net/socket.h>
#include <sys/socket.h>
#include <kernel/thread/event.h>
#include <net/skbuff.h>
#include <linux/aio.h>

struct proto;
struct sk_buff_head;
struct socket;
struct kiocb;
struct msghdr;
struct skbuff;

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
	unsigned char skc_state;
#if 0
	unsigned char skc_reuse;
	int skc_bound_dev_if;
#endif
	struct proto *skc_prot;
};

/**
 * Network layer representation of sockets.
 * @param __sk_common shared layout with inet_timewait_sock
 * @param sk_protocol which protocol this socket belongs in this network family
 * @param sk_shutdown mask of SHUT_RD and SHUT_RW
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
 * @param sk_encap_rcv: called before put skbuff data on socket. Handle encapsulated proto
 * @param get_port TODO add description
 * @param arp_queue_info: arp_queue related parameter
 * @param sock_is_ready: event for waking up socket, when the packet is added to arp_queue
 * @param sock_is_not_empty: event for waking up in recvfrom(), when some data were put on sock
 */
typedef struct sock {
	struct sock_common __sk_common;
#define sk_family  __sk_common.skc_family
#define sk_prot    __sk_common.skc_prot
#define sk_state   __sk_common.skc_state
	unsigned char sk_protocol;
	unsigned char sk_shutdown;
	unsigned short sk_type;
	socket_lock_t sk_lock;

#if 0
	struct {
		struct sk_buff *head;
		struct sk_buff *tail;
	} sk_backlog;

	int sk_sndbuf;
	int sk_rcvbuf;
	unsigned long sk_flags;
#endif

	struct sk_buff_head *sk_receive_queue;
	struct sk_buff_head *sk_write_queue;

	struct socket *sk_socket;
#if 0
	void *sk_user_data;

	void (* sk_state_change)(struct sock *sk);
	void (* sk_data_ready)(struct sock *sk, int bytes);
	void (* sk_write_space)(struct sock *sk);
	void (* sk_error_report)(struct sock *sk);
	int (* sk_backlog_rcv)(struct sock *sk, sk_buff_t *pack);
	void (* sk_destruct)(struct sock *sk);
	int (* get_port)(struct sock *sk, unsigned short num); // TODO
#endif
	int (*sk_encap_rcv)(struct sock *sk, struct sk_buff *pack);

	int32_t sk_err;

#if 0
	int ready;
	struct event sock_is_ready;
#endif

	struct event sock_is_not_empty;
} sock_t;

#if 0
static inline void sock_set_ready(struct sock *sk) {
	sk->ready = true;
}
static inline void sock_unset_ready(struct sock *sk){
	sk->ready = false;
}

/**
 *	Check if socket is awaiting address resolution
 *	@return
 *         - true if socket is ready for further actions
 *         - false in other case
 */
static inline bool sock_is_ready(struct sock *sk) {
	return sk->ready;
}
#endif

#if 0
/** Sock flags */
enum sock_flags {
	SOCK_DEAD,
	SOCK_DONE,
	SOCK_DESTROY,
	SOCK_BROADCAST
};
#endif

/** Protocol specific functions */
typedef struct proto {
	void (*close)(sock_t *sk, long timeout);
	int (*shutdown)(struct sock *sk, int flags);
	int (*connect)(sock_t *sk, sockaddr_t *addr, int addr_len);
#if 0
	int (*disconnect)(sock_t *sk, int flags);
#endif
	int (*listen)(sock_t *sk, int backlog);
	int (*accept)(sock_t *sk, sock_t **newsk, sockaddr_t *addr, int *addr_len);
	int (*ioctl)(struct sock *sk, int cmd, unsigned long arg);
	int (*init)(sock_t *sk);
#if 0
	void (*destroy)(struct sock *sk);
#endif
	int (*setsockopt)(struct sock *sk, int level, int optname, char *optval,
			int optlen);
	int (*getsockopt)(struct sock *sk, int level, int optname, char *optval,
			int *option);
	int (*sendmsg)(struct kiocb *iocb, sock_t *sk, struct msghdr *msg,
			size_t len);
	int (*recvmsg)(struct kiocb *iocb, sock_t *sk, struct msghdr *msg,
			size_t len, int noblock, int flags);
	int (*bind)(sock_t *sk, sockaddr_t *uaddr, int addr_len);
	int (*backlog_rcv)(sock_t *sk, sk_buff_t *skb);
	void (*hash)(struct sock *sk);
	void (*unhash)(struct sock *sk);
	sock_t *(*sock_alloc)(void); /**< if not NULL, allocate proto socket casted to sock_t */
	void (*sock_free)(sock_t *); /**< must not be NULL if sock_alloc is not NULL */
	unsigned int obj_size;
	struct cache *cachep;             /**< associated cache in which socks will be stored */
	char name[32];
} proto_t;

/** Initializes structure sock's pull */
extern void sk_init(void);

/** Allocates structure sock with specific parameters
 * @family - Protocol family (PF_INIT for example)
 * @priority - isn't used now
 * @prot - pointer to the proto structure
 */
extern sock_t *sk_alloc(int family, gfp_t priority, struct proto *prot);

/**
 * Returns specified structure sock into pull,
 * assuming there are no more handle on it.
 */
extern void sk_free(struct sock *sk);

/** This function used by all transports to attempt to queue received packets*/
extern void sock_queue_rcv_skb(struct sock *sk, struct sk_buff *skb);

/**
 * Functions to fill in entries in struct proto_ops when a protocol
 * does not implement a particular function.
 */
extern int sock_no_listen(struct socket *, int);
extern int sock_no_accept(struct socket *, struct socket *, int);

extern int sock_common_recvmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *msg, size_t size, int flags);

extern void sk_common_release(struct sock *sk);

/* Simple spinlock */
extern int sock_lock(struct sock **psk);
extern void sock_unlock(struct sock *sk);

/**
 * clear sk_err variable in struct sock
 **/
static inline void sk_clear_pending_error(struct sock *sk){
	sk->sk_err = 0;
}

#endif /* NET_SOCK_H_ */
