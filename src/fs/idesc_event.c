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

struct idesc_waitq_link {
	int iwq_masks;
	struct dlist_head iwq_link;
	struct thread *iwq_thread;
};

POOL_DEF(idesc_event_pool, struct idesc_waitq_link, 100);

int idesc_waitq_prepare(struct idesc * idesc, int mask) {
	struct idesc_waitq_link *link;

	link = pool_alloc(&idesc_event_pool);
	if (!link) {
		return -ENOMEM;
	}
	dlist_init(&link->iwq_link);
	dlist_add_next(&link->iwq_link, &idesc->idesc_event_list);

	link->iwq_thread = thread_self();

	return 0;
}

int idesc_notify_all(struct idesc * idesc, int mask) {
	struct idesc_waitq_link *tmp, *link;

	dlist_foreach_entry(link, tmp, &idesc->idesc_event_list, iwq_link) {
		if (link->iwq_masks & mask) {
			//sched_notify(link->iwq_thread);
		}
	}

	return 0;
}

void idesc_wait_prepare(struct idesc *idesc, struct idesc_wait_link *wl,
		int mask) {

}

int idesc_wait(struct idesc_wait_link *wl, unsigned int timeout) {

#if 0
	if (idesc->idesc_flags & O_NONBLOCK) {
		return -EAGAIN;
	}
#endif

	/* do actual wait */
	return -ENOTSUP;
}

void idesc_wait_cleanup(struct idesc_wait_link *wl) {

}
