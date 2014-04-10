/**
 * @file
 *
 * @date Nov 7, 2013
 * @author: Anton Bondarev
 */
#include <net/sock.h>
#include <util/list.h>

void sock_hash(struct sock *sk) {
	assert(sk != NULL);
	assert(sk->p_ops != NULL);
	assert(list_alone_element(sk, lnk));

	list_add_last_element(sk, sk->p_ops->sock_list, lnk);
}

void sock_unhash(struct sock *sk) {
	assert(sk != NULL);
	assert(!list_alone_element(sk, lnk));

	list_unlink_element(sk, lnk);
}
