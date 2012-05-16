/**
 * @file
 * @brief Generic socket support routines.
 * @details Contains functions for working with all type of structure sock.
 *			It includes free sock pull.
 *
 * @date 12.08.09
 * @author Nikolay Korotky
 */

#include <errno.h>
#include <net/skbuff.h>
#include <net/sock.h>
#include <mem/misc/pool.h>
#include <hal/ipl.h>

#define LOWER_BOUND 1 // lower bound of sockets count

#if 0
typedef struct sock_info {
	/*FIXME NETSOCK: now we use just udp_sock pull. It is the biggest of sock
	 *  structure now. But we must allocate sock with size equals obj_size
	 *  member in proto structure.
	 */
	struct udp_sock sk;
} sock_info_t __attribute__ ((aligned (4)));

/* pool for allocate sock_info */
POOL_DEF(socks_pool, sock_info_t, CONFIG_MAX_KERNEL_SOCKETS);
#endif


/* allocates proto structure for specified protocol*/
static struct sock * sk_prot_alloc(struct proto *prot, gfp_t priority) {
	struct sock *sk;
	ipl_t sp;

	assert(prot != NULL);

	sk = NULL;
	sp = ipl_save();
	if (prot->sock_alloc != NULL) {
		assert(prot->sock_free != NULL);
		sk = prot->sock_alloc();
	}
	else {
		if (prot->cachep == NULL) {
			prot->cachep = cache_create(prot->name, prot->obj_size, LOWER_BOUND);
		}
		if (prot->cachep != NULL) {
			sk = cache_alloc(prot->cachep);
		}
	}
	ipl_restore(sp);

	return sk;
}

/* returns specified structure sock into pull */
static void sk_prot_free(const struct proto *prot, struct sock *sk) {
	ipl_t sp;

	assert(prot != NULL);
	assert(sk != NULL);

	sp = ipl_save();
	if (prot->sock_free != NULL) {
		assert(prot->sock_alloc != NULL);
		prot->sock_free(sk);
	} else {
		assert(prot->cachep != NULL);
		cache_free(prot->cachep, sk);
	}
	ipl_restore(sp);
}

struct sock * sk_alloc(int family, gfp_t priority, struct proto *prot) {
	struct sock *sk;

	assert(prot != NULL);

	sk = sk_prot_alloc(prot, 0);
	if (sk == NULL) {
		return NULL;
	}

	sk->sk_receive_queue = alloc_skb_queue();
	if (sk->sk_receive_queue == NULL) {
		sk_prot_free(prot, sk);
		return NULL;
	}

	sk->sk_write_queue = alloc_skb_queue();
	if (sk->sk_write_queue == NULL) {
		skb_queue_purge(sk->sk_receive_queue);
		sk_prot_free(prot, sk);
		return NULL;
	}

	if (prot->init != NULL) {
		if (prot->init(sk) < 0) {
			skb_queue_purge(sk->sk_receive_queue);
			skb_queue_purge(sk->sk_write_queue);
			sk_prot_free(prot, sk);
			return NULL;
		}
	}

	sk->sk_destruct = NULL;
	sk->sk_family = family;
	sk->sk_prot = prot;

	if (prot->hash != NULL) {
		prot->hash(sk);
	}

	return sk;
}

void sk_free(struct sock *sk) {
	assert(sk != NULL);
	assert(sk->sk_prot != NULL);

	if (sk->sk_prot->unhash != NULL) {
		sk->sk_prot->unhash(sk);
	}
	if (sk->sk_destruct != NULL) {
		sk->sk_destruct(sk);
	}
	sk_prot_free(sk->sk_prot, sk);
}

int sock_no_listen(struct socket *sock, int backlog) {
	assert(sock != NULL);
	return -EOPNOTSUPP;
}

int sock_no_accept(struct socket *sock, struct socket *newsock, int flags) {
	assert(sock != NULL);
	return -EOPNOTSUPP;
}

int sock_queue_rcv_skb(struct sock *sk, struct sk_buff *skb) {
	assert(sk != NULL);
	assert(sk->sk_receive_queue != NULL);
	assert(skb != NULL);

	skb_queue_tail(sk->sk_receive_queue, skb);
	return ENOERR;
}

int sock_common_recvmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *msg, size_t size, int flags) {
	assert(sock != NULL);
	assert(sock->sk != NULL);
	assert(sock->sk->sk_prot != NULL);
	assert(sock->sk->sk_prot->recvmsg != NULL);

	return sock->sk->sk_prot->recvmsg(iocb, sock->sk, msg, size, 0, flags);
}

void sk_common_release(struct sock *sk) {
	assert(sk != NULL);
	assert(sk->sk_prot != NULL);
	assert(sk->sk_receive_queue);
	assert(sk->sk_write_queue);

	if (sk->sk_prot->destroy != NULL) {
		sk->sk_prot->destroy(sk);
	}

	skb_queue_purge(sk->sk_receive_queue);
	skb_queue_purge(sk->sk_write_queue);

	sk_free(sk);
}

static int test_and_set(unsigned long *a) {
	register int tmp;

	sched_lock();
	tmp = *a;
	*a = 1;
	sched_unlock();

	return tmp;
}

void sock_lock(struct sock *sk) {
	assert(sk != NULL);
	while(test_and_set(&sk->sk_lock.slock));
}

void sock_unlock(struct sock *sk) {
	assert(sk != NULL);
	sk->sk_lock.slock = 0;
}
