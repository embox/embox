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
#include <kernel/event.h>
#include <net/skbuff.h>
#include <sys/time.h>

struct proto;
struct sk_buff_head;
struct socket;
struct msghdr;
struct skbuff;
struct pool;

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
	struct net_device *so_bindtodev;
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
	struct sock_opt opt;
	struct sk_buff_head rx_queue;
	struct sk_buff_head tx_queue;
	unsigned char state;
	unsigned char shutdown_flag;
	const struct sock_ops *ops;

	struct socket *sk_socket;
	int (*sk_encap_rcv)(struct sock *sk, struct sk_buff *pack);
	struct event sock_is_not_empty;
};

struct sock_ops {
	int (*close)(struct sock *sk);
	int (*connect)(struct sock *sk, const struct sockaddr *addr,
			socklen_t addrlen, int flags);
	int (*listen)(struct sock *sk, int backlog);
	int (*accept)(struct sock *sk, struct sockaddr *addr,
			socklen_t *addrlen, int flags, struct sock **out_sk);
	int (*sendmsg)(struct sock *sk, struct msghdr *msg, int flags);
	int (*recvmsg)(struct sock *sk, struct msghdr *msg, int flags);
	int (*shutdown)(struct sock *sk, int how);
	int (*setsockopt)(struct sock *sk, int level, int optname,
			const void *optval, socklen_t optlen);
	int (*getsockopt)(struct sock *sk, int level, int optname,
			void *optval, socklen_t *optlen);
	int (*init)(struct sock *sk);
	void (*hash)(struct sock *sk);
	void (*unhash)(struct sock *sk);
	struct sock * (*iter)(struct sock *sk);
	struct pool *obj_pool;
};

extern int sock_create(int family, int type, int protocol,
		const struct sock_ops *ops, struct sock **out_sk);
extern void sock_release(struct sock *sk);
extern void sock_rcv(struct sock *sk, struct sk_buff *skb);

#endif /* NET_SOCK_H_ */
