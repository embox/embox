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

#include <kernel/event.h>

struct idesc_event {
	int ie_masks;
	struct dlist_head idesc_event_link;
	struct event *idesc_event;
};

POOL_DEF(idesc_event_pool, struct idesc_event, 100);

int idesc_event_add(struct idesc * idesc, struct event *e, int mask) {
	struct idesc_event *link;

	link = pool_alloc(&idesc_event_pool);
	if (!link) {
		return -ENOMEM;
	}
	dlist_init(&link->idesc_event_link);
	dlist_add_next(&link->idesc_event_link, &idesc->idesc_event_list);

	return -ENOSUPP;
}

int idesc_notify_all(struct idesc * idesc, int mask) {
	struct idesc_event *tmp, *event;

	dlist_foreach_entry(event, tmp, &idesc->idesc_event_list, idesc_event_link) {
		if (event->ie_masks & mask) {
			event_notify(event->idesc_event);
		}
	}

	return 0;
}
