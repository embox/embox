/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 * @author: Anton Kozlov
 */

#include <errno.h>

#include <mem/misc/pool.h>
#include <lib/libds/dlist.h>
#include <kernel/task/resource/idesc.h>
#include <fcntl.h>
#include <kernel/sched.h>

#include <kernel/task/resource/idesc_event.h>

int idesc_wait_prepare(struct idesc *i, struct idesc_wait_link *wl) {

	waitq_wait_prepare(&i->idesc_waitq, &wl->link);

	if (i->idesc_flags & O_NONBLOCK) {
		return -EAGAIN;
	}

	return 0;
}

int idesc_notify(struct idesc *idesc, int mask) {

	//TODO MASK
	waitq_wakeup(&idesc->idesc_waitq, 0);

	return 0;
}

void idesc_wait_cleanup(struct idesc *i, struct idesc_wait_link *wl) {
	waitq_wait_cleanup(&i->idesc_waitq, &wl->link);
}
