/**
 * @file
 *
 * @date Nov 7, 2013
 * @author: Anton Bondarev
 */
#include <net/sock.h>
#include <util/list.h>

void sock_hash(struct sock *sk) {
	if (sk == NULL) {
		return; /* error: invalid argument */
	}
	else if (!list_alone_element(sk, lnk)) {
		return; /* error: already in hash */
	}

	assert(sk->p_ops != NULL);
	list_add_last_element(sk, sk->p_ops->sock_list, lnk);
}

void sock_unhash(struct sock *sk) {
	if (sk == NULL) {
		return; /* error: invalid argument */
	}
	else if (list_alone_element(sk, lnk)) {
		return; /* error: not hashed */
	}

	list_unlink_element(sk, lnk);
}
