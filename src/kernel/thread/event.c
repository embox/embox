/**
 * @file
 * @brief TODO
 *
 * @date 07.09.2012
 * @author Anton Bulychev
 */

#include <kernel/thread/event.h>
#include <kernel/thread/sched.h>

void event_init(struct event *e, const char *name) {
	wait_queue_init(&e->waitq);
	e->name = name;
}

int event_wait(struct event *e, unsigned long timeout) {
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if (critical_allows(CRITICAL_SCHED_LOCK)) {
		return wait_queue_wait(&e->waitq, timeout);
	} else {
		return wait_queue_wait_locked(&e->waitq, timeout);
	}
}

void event_notify(struct event *e) {
	wait_queue_notify_all(&e->waitq);
}

#if 0

static inline const char *event_name(struct event *e) {
	return e->name;
}

#endif
