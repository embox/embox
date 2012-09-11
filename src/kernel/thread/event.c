/**
 * @file
 * @brief TODO
 *
 * @date 07.09.2012
 * @author Anton Bulychev
 */

#include <kernel/thread/event.h>
#include <kernel/thread/sched.h>

#define EVENT_STATUS_NONE     0
#define EVENT_STATUS_WAITING  1
#define EVENT_STATUS_HAPPEND  2

void event_init(struct event *e, const char *name) {
	sleepq_init(&e->sleepq);
	e->status = EVENT_STATUS_NONE;
	e->name = name;
}

int event_wait(struct event *e, unsigned long timeout) {
	int res = 0;

	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	sched_lock();
	{
		if (e->status == EVENT_STATUS_HAPPEND) {
			e->status = EVENT_STATUS_NONE;
		} else {
			e->status = EVENT_STATUS_WAITING;
			res = sched_sleep_locked(&e->sleepq, timeout);
		}
	}
	sched_unlock();

	return res;
}

void event_notify(struct event *e) {
	sched_lock();
	{
		if (e->status == EVENT_STATUS_WAITING) {
			e->status = EVENT_STATUS_NONE;
			sched_wake_all(&e->sleepq);
		} else {
			e->status = EVENT_STATUS_HAPPEND;
		}
	}
	sched_unlock();
}

void event_clear(struct event *e) {
	sched_lock();
	{
		if (e->status == EVENT_STATUS_HAPPEND) {
			e->status = EVENT_STATUS_NONE;
		}
	}
	sched_unlock();
}

#if 0


static inline const char *event_name(struct event *e) {
	return e->name;
}

void event_notify_one(struct event *e) {
	sched_wake_one(&e->sleepq);
}
#endif

