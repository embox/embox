/**
 * @file
 *
 * @date Dec 2, 2013
 * @author: Anton Bondarev
 */
#include <fs/idesc.h>
#include <fs/idesc_event.h>
#include <kernel/time/time.h>
#include <kernel/softirq_lock.h>
#include <kernel/sched.h>
#include <net/sock.h>
#include <net/sock_wait.h>

int sock_wait(struct sock *sk, int flags, int timeout) {
	struct idesc_wait_link wl;

	if (timeout == 0) {
		timeout = SCHED_TIMEOUT_INFINITE;
	}

	return IDESC_WAIT_LOCKED(softirq_unlock(),
			&sk->idesc, &wl, flags, timeout,
			softirq_lock());
}
