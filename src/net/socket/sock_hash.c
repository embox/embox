/**
 * @file
 *
 * @date Nov 7, 2013
 * @author: Anton Bondarev
 */
#include <net/sock.h>
#include <lib/libds/dlist.h>
#include <hal/ipl.h>

void sock_hash(struct sock *sk) {
	ipl_t ipl;

	assert(sk != NULL);
	assert(sk->p_ops != NULL);
	assert(dlist_empty_entry(sk, lnk));

	/* TODO Probably, it's better to use spinlock here */
	ipl = ipl_save();
	dlist_add_prev_entry(sk, sk->p_ops->sock_list, lnk);
	ipl_restore(ipl);
}

void sock_unhash(struct sock *sk) {
	ipl_t ipl;

	assert(sk != NULL);
	assert(!dlist_empty_entry(sk, lnk));

	ipl = ipl_save();
	dlist_del_init_entry(sk, lnk);
	ipl_restore(ipl);
}
