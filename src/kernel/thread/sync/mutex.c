/**
 * @file
 * @brief Implements mutex methods.
 *
 * @date 05.05.10
 * @author Nikolay Malkovsky, Kirill Skorodumov
 *          - Initial contribution
 * @author Alina Kramar
 *          - Priority inheritance
 * @author Eldar Abusalimov
 *          - Debugging and code cleanup
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include <hal/ipl.h>
#include <kernel/thread/state.h>
#include <kernel/thread.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/sched.h>


static int trylock_sched_locked(struct mutex *m, struct thread *current);

static int priority_inherit(struct thread *t, struct mutex *m);
static void priority_uninherit(struct thread *t);

static inline int mutex_is_static_inited(struct mutex *m) {
	/* Static initializer can't really init list now, so if this condition's
	 * true initialization is not finished */
	if(!(m->wq.list.next && m->wq.list.prev)) {
		return 1;
	}
	return 0;
}

static inline void mutex_complete_static_init(struct mutex *m) {
	wait_queue_init(&m->wq);
}

void mutex_init_default(struct mutex *m, const struct mutexattr *attr) {
	wait_queue_init(&m->wq);
	m->lock_count = 0;
	m->holder = NULL;

	if (attr) {
		mutexattr_copy(attr, &m->attr);
	} else {
		mutexattr_init(&m->attr);
	}
}

void mutex_init(struct mutex *m) {
	mutex_init_default(m, NULL);
	mutexattr_settype(&m->attr, MUTEX_RECURSIVE | MUTEX_ERRORCHECK);
}

int mutex_lock(struct mutex *m) {
	int ret = 0;
	struct thread *current = thread_self();

	assert(m);
	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		while ((ret = trylock_sched_locked(m, current)) != 0) {
			if (ret == -EAGAIN && (m->attr.type & MUTEX_ERRORCHECK)){
				goto out;
			}
			/* We have to wait for a mutex to be released. */
			priority_inherit(current, m);
			wait_queue_wait_locked(&m->wq, SCHED_TIMEOUT_INFINITE); /* Sleep here... */
		}
	}

out:
	sched_unlock();
	return ret;
}

int mutex_trylock(struct mutex *m) {
	int ret;
	struct thread *current = thread_self();

	assert(m);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	sched_lock();
	{
		ret = trylock_sched_locked(m, current);
	}
	sched_unlock();

	return ret;
}

static int trylock_sched_locked(struct mutex *m, struct thread *current) {
	assert(m && current);
	assert(critical_inside(CRITICAL_SCHED_LOCK));

	if(mutex_is_static_inited(m)) {
		mutex_complete_static_init(m);
	}

	if (m->holder == current) {
		if (m->attr.type & MUTEX_RECURSIVE){
			/* Nested locks. */
			m->lock_count++;
			return 0;
		}
		if (m->attr.type & MUTEX_ERRORCHECK){
			/* Nested locks. */
			return -EAGAIN;
		}
	}

	if (m->holder) {
		return -EBUSY;
	}

	m->lock_count = 1;
	m->holder = current;

	return 0;
}

int mutex_unlock(struct mutex *m) {
	int error = 0;
	struct thread *current = thread_self();

	assert(m);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	sched_lock();
	{
		if ((!m->holder || m->holder != current) &&
				(m->attr.type & (MUTEX_ERRORCHECK | MUTEX_RECURSIVE))){
			error = EPERM;
			goto out;
		}

		assert(m->lock_count > 0);

		if(--m->lock_count != 0  && (m->attr.type & MUTEX_RECURSIVE)) {
			goto out;
		}

		priority_uninherit(current);

		m->holder = NULL;
		m->lock_count = 0;
		wait_queue_notify(&m->wq);
	}
out:
	sched_unlock();
	return -error;
}

static int priority_inherit(struct thread *t, struct mutex *m) {
	sched_priority_t prior;

	assert(t);
	assert(critical_inside(CRITICAL_SCHED_LOCK));

	prior = thread_priority_get(t);

	if(prior != thread_priority_inherit(m->holder, prior)) {
		sched_change_priority(m->holder, prior);
	}

	return 0;
}

static void priority_uninherit(struct thread *t) {
	sched_priority_t prior;

	assert(t);
	assert(critical_inside(CRITICAL_SCHED_LOCK));

	prior = thread_priority_get(t);

	if(prior != thread_priority_reverse(t)) {
		sched_change_priority(t, thread_priority_get(t));
	}
}
