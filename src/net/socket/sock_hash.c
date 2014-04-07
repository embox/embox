/**
 * @file
 *
 * @date Nov 7, 2013
 * @author: Anton Bondarev
 */
#include <net/sock.h>
#include <util/dlist.h>

void sock_hash(struct sock *sk) {
	assert(sk != NULL);
	assert(sk->p_ops != NULL);
	assert(dlist_empty(&sk->lnk));

	dlist_add_prev(&sk->lnk, sk->p_ops->sock_list);
}

void sock_unhash(struct sock *sk) {
	assert(sk != NULL);
	assert(!dlist_empty(&sk->lnk));

	dlist_del(&sk->lnk);
}
