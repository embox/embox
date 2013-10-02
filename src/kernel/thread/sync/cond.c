/**
 * @file
 * @brief Implements condition variable methods.
 *
 * @date 03.09.2012
 * @author Anton Bulychev
 */

#include <assert.h>
#include <errno.h>
#include <time.h>
#include <kernel/thread/sync/cond.h>
#include <kernel/sched.h>
#include <kernel/thread/types.h>
#include <kernel/thread.h>
#include <kernel/time/time.h>

static void condattr_copy(const struct condattr *source, struct condattr *dest) {
	dest->pshared = source->pshared;
}

void cond_init(cond_t *c, const struct condattr *attr) {
	struct thread* current = thread_self();
	wait_queue_init(&c->wq);
	condattr_init(&c->attr);
	c->task = current->task;
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

int cond_wait(cond_t *c, struct mutex *m) {
	return cond_timedwait(c, m, NULL);
}

int cond_timedwait(cond_t *c, struct mutex *m, const struct timespec *ts) {
	struct thread* current = thread_self();
	int timeout;
	time64_t time;

	assert(c && m);
	assert(critical_allows(CRITICAL_SCHED_LOCK));

	if ((current->task != c->task) && (c->attr.pshared == PROCESS_PRIVATE)) {
		return -EACCES;
	}
	if(ts == NULL) {
		timeout = SCHED_TIMEOUT_INFINITE;
	} else {
		time = timespec_to_ns(ts);
		timeout = (int) time; //TODO overflow
	}

	sched_lock();
	{
		mutex_unlock(m);
		wait_queue_wait_locked(&c->wq, timeout);
	}
	sched_unlock();

	mutex_lock(m);

	return ENOERR;
}

int cond_signal(cond_t *c) {
	struct thread* current = thread_self();

	assert(c);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if ((current->task != c->task) && (c->attr.pshared == PROCESS_PRIVATE)) {
		return -EACCES;
	}

	wait_queue_notify(&c->wq);

	return ENOERR;
}

int cond_broadcast(cond_t *c) {
	struct thread* current = thread_self();

	assert(c);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if ((current->task != c->task) && (c->attr.pshared == PROCESS_PRIVATE)) {
		return -EACCES;
	}

	wait_queue_notify_all(&c->wq);

	return ENOERR;
}
