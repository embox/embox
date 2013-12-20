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

int idesc_wait_do_prepare(struct idesc *i, struct idesc_wait_link *wl) {

	assert(i);
	assert(wl);

	waitq_wait_prepare(&i->idesc_waitq, &wl->link);

	return 0;
}

int idesc_wait_prepare(struct idesc *i, struct idesc_wait_link *wl) {

	if (i->idesc_flags & O_NONBLOCK) {
		return -EAGAIN;
	}

	return idesc_wait_do_prepare(i, wl);
}

int idesc_notify(struct idesc *idesc, int mask) {

	//TODO MASK
	waitq_wakeup(&idesc->idesc_waitq, 0);

	return 0;
}

void idesc_wait_cleanup(struct idesc *i, struct idesc_wait_link *wl) {
	waitq_wait_cleanup(&i->idesc_waitq, &wl->link);
}

int idesc_wait(struct idesc *idesc, unsigned int timeout) {
	return sched_wait_timeout(timeout, NULL);
}

