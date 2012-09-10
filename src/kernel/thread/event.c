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
	e->flag = 0;
	e->name = name;
}

int event_wait(struct event *e, unsigned long timeout) {
	int res = 0;

	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	sched_lock();
	{
		if (e->flag) {
			e->flag = 0;
		} else {
			res = sched_sleep_locked(&e->sleepq, timeout);
		}
	}
	sched_unlock();

	return res;
}

void event_notify(struct event *e) {
	sched_lock();
	{
		if (sleepq_empty(&e->sleepq)) {
			e->flag = 1;
		} else {
			sched_wake_all(&e->sleepq);
		}
	}
	sched_unlock();
}

void event_clear(struct event *e) {
	e->flag = 0;
}

#if 0


static inline const char *event_name(struct event *e) {
	return e->name;
}

void event_notify_one(struct event *e) {
	sched_wake_one(&e->sleepq);
}
#endif

