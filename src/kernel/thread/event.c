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

int event_wait_ns(struct event *e, unsigned long timeout) {
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if (critical_allows(CRITICAL_SCHED_LOCK)) {
		return sched_sleep_ns(&e->sleepq, timeout);
	} else {
		return sched_sleep_locked_ns(&e->sleepq, timeout);
	}
}

int event_wait_us(struct event *e, unsigned long timeout) {
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if (critical_allows(CRITICAL_SCHED_LOCK)) {
		return sched_sleep_us(&e->sleepq, timeout);
	} else {
		return sched_sleep_locked_us(&e->sleepq, timeout);
	}
}

int event_wait_ms(struct event *e, unsigned long timeout) {
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if (critical_allows(CRITICAL_SCHED_LOCK)) {
		return sched_sleep_ms(&e->sleepq, timeout);
	} else {
		return sched_sleep_locked_ms(&e->sleepq, timeout);
	}
}

void event_notify(struct event *e) {
	sched_wake_all(&e->sleepq);
}

#if 0

static inline const char *event_name(struct event *e) {
	return e->name;
}

#endif
