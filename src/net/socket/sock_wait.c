/**
 * @file
 *
 * @date Dec 2, 2013
 * @author: Anton Bondarev
 */

#include <fs/idesc.h>
#include <fs/idesc_event.h>
#include <kernel/time/time.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/sched.h>
#include <net/sock.h>
#include <net/sock_wait.h>
#include <kernel/thread/thread_sched_wait.h>

int sock_wait(struct sock *sk, int flags, int timeout) {
	struct idesc_wait_link wl;

	if (timeout == 0) {
		timeout = SCHED_TIMEOUT_INFINITE;
	}

	return IDESC_WAIT_LOCKED(sched_unlock(),
			&sk->idesc, &wl, flags, timeout,
			sched_lock());
}

void sock_notify(struct sock *sk, int flags) {
	idesc_notify(&sk->idesc, flags);
}
