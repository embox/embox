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

int sock_wait(struct sock *sk, int flags) {
	struct idesc_wait_link wl;
	int res;
	unsigned long timeout;

	timeout = timeval_to_ms(&sk->opt.so_rcvtimeo);
	if (timeout == 0) {
		timeout = SCHED_TIMEOUT_INFINITE;
	}


	idesc_wait_prepare(&sk->idesc, &wl, flags);
	softirq_unlock();

	res = idesc_wait(&sk->idesc, timeout);
	softirq_lock();

	idesc_wait_cleanup(&sk->idesc, &wl);
	__waitq_cleanup();

	return res;
}
