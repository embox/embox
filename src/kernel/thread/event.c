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
	sleepq_init(&e->sleepq);
	e->name = name;
}

#if 0
static inline const char *event_name(struct event *e) {
	return e->name;
}
#endif

int event_wait(struct event *e, unsigned long timeout) {
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if (critical_allows(CRITICAL_SCHED_LOCK)) {
		return sched_sleep(&e->sleepq, timeout);
	} else {
		return sched_sleep_locked(&e->sleepq, timeout);
	}
}

void event_notify(struct event *e) {
	sched_wake_one(&e->sleepq);
}

void event_notify_all(struct event *e) {
	sched_wake(&e->sleepq);
}

