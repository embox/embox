/**
 * @file
 *
 * @date Dec 2, 2013
 * @author: Anton Bondarev
 */
#include <fs/idesc.h>
#include <fs/idesc_event.h>
#include <kernel/softirq_lock.h>
#include <kernel/sched.h>

int sock_wait(struct idesc *idesc, int flags) {
	struct idesc_wait_link wl;
	int res;

	idesc_wait_prepare(idesc, &wl, flags);
	softirq_unlock();
	//TODO must be locked (mutex?)

	res = idesc_wait(idesc, SCHED_TIMEOUT_INFINITE);
	softirq_lock();

	idesc_wait_cleanup(idesc, &wl);

	return res;
}
