/**
 * @file
 * @brief TODO
 *
 * @date 07.09.2012
 * @author Anton Bulychev
 */

#include <kernel/thread/event.h>
#include <kernel/thread/sched.h>
#include <types.h>
#include <mem/objalloc.h>

#define EVENT_SETS_CNT 64

OBJALLOC_DEF(event_set_pool, struct event_set, EVENT_SETS_CNT);

void event_set_init(struct event_set *e_set) {
	sleepq_init(&e_set->sleepq);
	dlist_init(&e_set->link);
}

struct event_set *event_set_create(void) {
	struct event_set *e_set = objalloc(&event_set_pool);

	sleepq_init(&e_set->sleepq);
	dlist_init(&e_set->link);

	return e_set;
}

void event_set_clear(struct event_set *e_set) {
	struct event *e, *nxt;

	dlist_foreach_entry(e, nxt, &e_set->link, link) {
		event_set_del(e);
	}

	objfree(&event_set_pool, e_set);
}

int event_wait(struct event_set *e_set, unsigned long timeout) {
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));
	assert(e_set);

	if (critical_allows(CRITICAL_SCHED_LOCK)) {
		return sched_sleep(&e_set->sleepq, timeout);
	} else {
		return sched_sleep_locked(&e_set->sleepq, timeout);
	}
}

void event_notify(struct event *e) {
	struct event_set *e_set = e->set;

	assert(e);

	sched_lock();
	{
		if (!e_set || sleepq_empty(&e_set->sleepq)) {
			goto out;
		}
		sched_wake_all(&e_set->sleepq);
		e_set->happened_event = e;
	}
out:
	sched_unlock();
}
