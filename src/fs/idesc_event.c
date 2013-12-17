/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>

#include <mem/misc/pool.h>
#include <util/dlist.h>
#include <fs/idesc.h>
#include <fcntl.h>
#include <kernel/thread.h>

#include <fs/idesc_event.h>

int idesc_wait_prepare(struct idesc *i, struct idesc_wait_link *wl, int mask) {
	assert(i);
	assert(wl);

	wl->iwq_masks = mask;
	waitq_link_init(&wl->link);
	waitq_wait_prepare(&i->idesc_waitq, &wl->link);

	return 0;
}

int idesc_notify(struct idesc *idesc, int mask) {

	//TODO MASK
	waitq_wakeup(&idesc->idesc_waitq, 0);

	return 0;
}

int idesc_wait(struct idesc *idesc, int mask, unsigned int timeout) {
	int ret;

	if (idesc->idesc_flags & O_NONBLOCK) {
		return -EAGAIN;
	}

	ret = SCHED_WAIT_TIMEOUT(idesc->idesc_ops->status(idesc, mask), timeout);

	/* do actual wait */
	return ret;
}

void idesc_wait_cleanup(struct idesc *i, struct idesc_wait_link *wl) {
//	struct thread *current_thd = thread_self();

//	assert(current_thd->wait_link == &wl->link);

	/* clean up current thread waitq. WAITING guaranted to cleared,
 	 * thread's wait_link removed from waitq
	 */
	//__waitq_cleanup();
	waitq_wait_cleanup(&i->idesc_waitq, &wl->link);
}
