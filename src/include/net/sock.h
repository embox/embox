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

#include <sys/socket.h>
#include <kernel/task/io_sync.h>
#include <net/skbuff.h>
#include <sys/time.h>
#include <util/list.h>
#include <stddef.h>
#include <stdbool.h>

struct family_ops;
struct sock_ops;
struct sk_buff_head;
struct sock;
struct msghdr;
struct skbuff;
struct pool;
struct indexator;
struct list;

typedef struct {
	spinlock_t slock;
} sock_lock_t;

#define SOCK_OPT_DEFAULT_RCVBUF   16384
#define SOCK_OPT_DEFAULT_RCVLOWAT 1
#define SOCK_OPT_DEFAULT_RCVTIMEO { .tv_sec = 0, .tv_usec = 0 }
#define SOCK_OPT_DEFAULT_SNDBUF   16384
#define SOCK_OPT_DEFAULT_SNDLOWAT 1
#define SOCK_OPT_DEFAULT_SNDTIMEO { .tv_sec = 0, .tv_usec = 0 }

struct sock_opt {
	int so_acceptconn;
	struct net_device *so_bindtodevice;
	int so_broadcast;
	int so_domain;
	int so_dontroute;
	int so_error;
	struct linger so_linger;
	int so_oobinline;
	int so_protocol;
	int so_rcvbuf;
	int so_rcvlowat;
	struct timeval so_rcvtimeo;
	int so_sndbuf;
	int so_sndlowat;
	struct timeval so_sndtimeo;
	int so_type;
};

struct sock {
	struct list_link lnk;
	struct sock_opt opt;
	struct sk_buff_head rx_queue;
	struct sk_buff_head tx_queue;
	//unsigned char state;
	unsigned char shutdown_flag;
	const struct family_ops *f_ops;
	const struct sock_ops *ops;
	struct io_sync ios;
    struct socket_node *sock_node;
};

struct family_ops {
	int (*init)(struct sock *sk);
	int (*close)(struct sock *sk);
	int (*bind)(struct sock *sk, const struct sockaddr *addr,
			socklen_t addrlen);
	int (*bind_local)(struct sock *sk);
	int (*connect)(struct sock *sk, const struct sockaddr *addr,
			socklen_t addrlen, int flags);
	int (*listen)(struct sock *sk, int len);
	int (*accept)(struct sock *sk, struct sockaddr *addr,
			socklen_t *addrlen, int flags, struct sock **out_sk);
	int (*sendmsg)(struct sock *sk, struct msghdr *msg,
			int flags);
	int (*recvmsg)(struct sock *sk, struct msghdr *msg,
			int flags);
	int (*getsockname)(struct sock *sk, struct sockaddr *addr,
			socklen_t *addrlen);
	int (*getpeername)(struct sock *sk, struct sockaddr *addr,
			socklen_t *addrlen);
	int (*getsockopt)(struct sock *sk, int level, int optname,
			void *optval, socklen_t *optlen);
	int (*setsockopt)(struct sock *sk, int level, int optname,
			const void *optval, socklen_t optlen);
	int (*shutdown)(struct sock *sk, int how);
	bool (*compare_addresses)(struct sockaddr *addr1, struct sockaddr *addr2);
};

struct sock_ops {
	int (*init)(struct sock *sk);
	int (*close)(struct sock *sk);
	int (*connect)(struct sock *sk, const struct sockaddr *addr,
			socklen_t addrlen, int flags);
	int (*listen)(struct sock *sk, int backlog);
	int (*accept)(struct sock *sk, struct sockaddr *addr,
			socklen_t *addrlen, int flags, struct sock **out_sk);
	int (*sendmsg)(struct sock *sk, struct msghdr *msg, int flags);
	int (*recvmsg)(struct sock *sk, struct msghdr *msg, int flags);
	int (*getsockopt)(struct sock *sk, int level, int optname,
			void *optval, socklen_t *optlen);
	int (*setsockopt)(struct sock *sk, int level, int optname,
			const void *optval, socklen_t optlen);
	int (*shutdown)(struct sock *sk, int how);
	struct pool *sock_pool;
	struct indexator *sock_port;
	struct list *sock_list;
};

typedef int (*sock_lookup_tester_ft)(const struct sock *sk,
		const struct sk_buff *skb);

extern int sock_create(int family, int type, int protocol,
		struct sock **out_sk);
extern int sock_create_ext(int family, int type, int protocol,
		int need_hash, struct sock **out_sk);
extern void sock_release(struct sock *sk);
extern void sock_hash(struct sock *sk);
extern void sock_unhash(struct sock *sk);
extern struct sock * sock_iter(const struct sock_ops *ops);
extern struct sock * sock_next(const struct sock *sk);
extern struct sock * sock_lookup(const struct sock *sk,
		const struct sock_ops *ops, sock_lookup_tester_ft tester,
		const struct sk_buff *skb);
extern void sock_rcv(struct sock *sk, struct sk_buff *skb,
		unsigned char *p_data, size_t size);
extern int sock_close(struct sock *sk);


extern int sock_common_recvmsg(struct sock *sk, struct msghdr *msg,
		int flags, int stream_mode);
static inline int sock_nonstream_recvmsg(struct sock *sk,
		struct msghdr *msg, int flags) {
	return sock_common_recvmsg(sk, msg, flags, 0);
}
static inline int sock_stream_recvmsg(struct sock *sk,
		struct msghdr *msg, int flags) {
	return sock_common_recvmsg(sk, msg, flags, 1);
}

#define sock_foreach(sk, ops) \
	list_foreach(sk, ops->sock_list, lnk)

#endif /* NET_SOCK_H_ */
