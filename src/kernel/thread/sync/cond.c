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
	sleepq_init(&c->sq);
}

void cond_wait(cond_t *c, struct mutex *m) {
	assert(c && m);
	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		mutex_unlock(m);
		sched_sleep_locked(&c->sq, SCHED_TIMEOUT_INFINITE);
	}
	sched_unlock();

	mutex_lock(m);
}

void cond_signal(cond_t *c) {
	assert(c);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	sched_wake_one(&c->sq);
}

void cond_broadcast(cond_t *c) {
	assert(c);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	sched_wake_all(&c->sq);
}

