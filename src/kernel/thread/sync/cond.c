/**
 * @file
 * @brief Implements condition variable methods.
 *
 * @date 03.09.2012
 * @author Anton Bulychev
 */

#include <assert.h>
#include <errno.h>
#include <kernel/thread/sync/cond.h>
#include <kernel/thread/sched.h>

void cond_init(cond_t *c) {
	event_init(&c->event, "cond");
}

void cond_wait(cond_t *c, struct mutex *m) {
	assert(c && m);
	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		mutex_unlock(m);
		sched_sleep_locked(&c->event, SCHED_TIMEOUT_INFINITE);
	}
	sched_unlock();

	mutex_lock(m);
}

void cond_signal(cond_t *c) {
	assert(c);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	sched_lock();
	{
		sched_wake_one(&c->event);
	}
	sched_unlock();
}

void cond_broadcast(cond_t *c) {
	assert(c);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	sched_lock();
	{
		sched_wake(&c->event);
	}
	sched_unlock();
}

