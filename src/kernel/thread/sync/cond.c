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
#include <kernel/sched.h>

static void condattr_copy(const struct condattr *source, struct condattr *dest) {
	dest->pshared = source->pshared;
}

void cond_init(cond_t *c, const struct condattr *attr) {
	wait_queue_init(&c->wq);
	condattr_init(&c->attr);
	if (attr) {
		condattr_copy(attr, &c->attr);
	}
}

void cond_destroy(cond_t *c) {

}

void condattr_init(struct condattr *attr) {
	attr->pshared = PROCESS_PRIVATE;
}

void condattr_destroy(struct condattr *attr) {

}

void condattr_getpshared(const struct condattr *attr, int *pshared) {
	*pshared = attr->pshared;
}

void condattr_setpshared(struct condattr *attr, int pshared) {
	attr->pshared = pshared;
}

void cond_wait(cond_t *c, struct mutex *m) {
	assert(c && m);
	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		mutex_unlock(m);
		wait_queue_wait_locked(&c->wq, SCHED_TIMEOUT_INFINITE);
	}
	sched_unlock();

	mutex_lock(m);
}

void cond_signal(cond_t *c) {
	assert(c);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	wait_queue_notify(&c->wq);
}

void cond_broadcast(cond_t *c) {
	assert(c);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	wait_queue_notify_all(&c->wq);
}
