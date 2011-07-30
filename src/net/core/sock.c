/**
 * @file
 * @brief Generic socket support routines.
 * @details Contains functions for working with all type of structure sock.
 *			It includes free sock pull.
 *
 * @date 12.08.09
 * @author Nikolay Korotky
 */

#include <string.h>
#include <errno.h>
#include <kernel/irq.h>
#include <net/skbuff.h>
#include <net/sock.h>
#include <net/udp.h>
#include <linux/init.h>
#include <util/array.h>
#include <mem/misc/pool.h>

#include <asm/system.h> /*linux-compatible*/


typedef struct sock_info {
	/*FIXME NETSOCK: now we use just udp_sock pull. It is the biggest of sock
	 *  structure now. But we must allocate sock with size equals obj_size
	 *  member in proto structure.
	 */
	struct udp_sock sk;
} sock_info_t __attribute__ ((aligned (4)));

/* pool for allocate sock_info */
POOL_DEF(socks_pool, sock_info_t, CONFIG_MAX_KERNEL_SOCKETS);


/* allocates proto structure for specified protocol*/
static struct sock *sk_prot_alloc(struct proto *prot, gfp_t priority,
		int family) {
	struct sock *sock;
	unsigned long flags;

	local_irq_save(flags);

	if((sock = pool_alloc(&socks_pool)) == NULL) {
		local_irq_restore(flags);
		return NULL;
	}

	prot->hash(sock);

	local_irq_restore(flags);

	return sock;
}

/* returns specified structure sock into pull */
static void sk_prot_free(struct proto *prot, struct sock *sk) {
	unsigned long irq_old;
	local_irq_save(irq_old);
	pool_free(&socks_pool, sk);
	prot->unhash(sk);
	local_irq_restore(irq_old);
}

struct sock *sk_alloc(/*struct net *net,*/int family, gfp_t priority,
		struct proto *prot) {
	struct sock *sk;
	sk = sk_prot_alloc(prot, 0, family);
	if (sk) {
		sk->sk_family = family;
		sk->sk_prot = prot;
		sk->sk_receive_queue = alloc_skb_queue(CONFIG_QUANTITY_SKB_QUEUE);
		sk->sk_write_queue = alloc_skb_queue(CONFIG_QUANTITY_SKB_QUEUE);
	}
	return sk;
}

void sk_free(struct sock *sk) {
	if (sk->sk_destruct) {
		sk->sk_destruct(sk);
	}
	sk_prot_free(sk->sk_prot, sk);
}

int sock_no_listen(struct socket *sock, int backlog) {
	return -EOPNOTSUPP;
}

int sock_no_accept(struct socket *sock, struct socket *newsock, int flags) {
	return -EOPNOTSUPP;
}

int sock_queue_rcv_skb(struct sock *sk, struct sk_buff *skb) {
	skb_queue_tail(sk->sk_receive_queue, skb);
	return 0;
}

int sock_common_recvmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *msg, size_t size, int flags) {
	struct sock *sk = sock->sk;
	return sk->sk_prot->recvmsg(iocb, sk, msg, size, 0, flags, NULL);
}

void sk_common_release(struct sock *sk) {
	if (sk->sk_prot->destroy) {
		sk->sk_prot->destroy(sk);
	}
	//sk->sk_prot->unhash(sk);
	sk_free(sk);
}
