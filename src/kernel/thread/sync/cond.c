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

#include <lib/libds/dlist.h>

#include <kernel/thread/sync/mutex.h>
#include <kernel/thread/sync/cond.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/sched.h>
#include <kernel/thread/types.h>
#include <kernel/thread.h>
#include <kernel/time/time.h>


static inline int cond_is_static_inited(cond_t *c) {
	/* Static initializer can't really init list now, so if this condition's
	 * true initialization is not finished */
	return !(c->wq.list.next && c->wq.list.prev);
}

static void condattr_copy(const struct condattr *source, struct condattr *dest) {
	dest->pshared = source->pshared;
}

void cond_init(cond_t *c, const struct condattr *attr) {
	struct thread* current;

	assert(c);

	current = thread_self();

	waitq_init(&c->wq);

	c->task = current->task;
	if (attr) {
		condattr_copy(attr, &c->attr);
	} else {
		condattr_init(&c->attr);
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

void condattr_getclock(const struct condattr *restrict attr,
		clockid_t *restrict clock_id) {
	*clock_id = attr->clock_id;
}

void condattr_setclock(struct condattr *attr, clockid_t clock_id) {
	attr->clock_id = clock_id;
}

int cond_wait(cond_t *c, struct mutex *m) {
	return cond_timedwait(c, m, NULL);
}

int cond_timedwait(cond_t *c, struct mutex *m, const struct timespec *ts) {
	struct thread* current = thread_self();
	struct timespec relative_time;
	clock_t timeout;
	int res;

	assert(c && m);
	assert(critical_allows(CRITICAL_SCHED_LOCK));

	if (!c->task) {
		c->task = current->task;
	}

	if ((current->task != c->task) && (c->attr.pshared == PROCESS_PRIVATE)) {
		return EACCES;
	}
	if (ts == NULL) {
		timeout = SCHED_TIMEOUT_INFINITE;
	} else {
		clock_gettime(CLOCK_REALTIME, &relative_time);
		relative_time = timespec_sub(*ts, relative_time);
		if ((relative_time.tv_nsec < 0) || (relative_time.tv_sec < 0)) {
			relative_time.tv_nsec = 0;
			relative_time.tv_sec = 0;
		}
		timeout = (clock_t) (timespec_to_ns(&relative_time) / NSEC_PER_MSEC); //TODO overflow
	}

	if (cond_is_static_inited(c)) {
		waitq_init(&c->wq);
	}

	res = ENOERR;
	sched_lock();
	{
		struct waitq_link *wql = &current->schedee.waitq_link;
		waitq_link_init(wql);
		waitq_wait_prepare(&c->wq, wql);

		mutex_unlock(m);

		res = sched_wait_timeout(timeout, NULL);

		waitq_wait_cleanup(&c->wq, wql);
	}
	sched_unlock();

	mutex_lock(m);

	/* RETURN VALUE
       All condition variable functions return 0 on success and a non-zero error code on error. */
	return -res;
}

int cond_signal(cond_t *c) {
	struct thread* current = thread_self();

	assert(c);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if (!c->task) {
		c->task = current->task;
	}

	if ((current->task != c->task) && (c->attr.pshared == PROCESS_PRIVATE)) {
		return -EACCES;
	}

	if (cond_is_static_inited(c)) {
		waitq_init(&c->wq);
	}

	waitq_wakeup(&c->wq, 0);

	return ENOERR;
}

int cond_broadcast(cond_t *c) {
	struct thread* current = thread_self();

	assert(c);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if (!c->task) {
		c->task = current->task;
	}

	if ((current->task != c->task) && (c->attr.pshared == PROCESS_PRIVATE)) {
		return -EACCES;
	}

	if (cond_is_static_inited(c)) {
		waitq_init(&c->wq);
	}

	waitq_wakeup_all(&c->wq);

	return ENOERR;
}
