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

#include <kernel/thread/sync/mutex.h>
#include <kernel/thread/waitq.h>

static inline int mutex_is_static_inited(struct mutex *m) {
	/* Static initializer can't really init list now, so if this condition's
	 * true initialization is not finished */
	return !(m->wq.list.next && m->wq.list.prev);
}

static inline void mutex_complete_static_init(struct mutex *m) {
	waitq_init(&m->wq);
}

void mutex_init_default(struct mutex *m, const struct mutexattr *attr) {
	waitq_init(&m->wq);
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
	mutexattr_settype(&m->attr, MUTEX_RECURSIVE);
}

int mutex_lock(struct mutex *m) {
	struct schedee *current = schedee_get_current();
	int errcheck;
	int ret, wait_ret;

	assert(m);

	errcheck = (m->attr.type == MUTEX_ERRORCHECK);

	wait_ret = WAITQ_WAIT(&m->wq, ({
		int done;

		sched_lock();
		ret = mutex_trylock(m);
		done = (ret == 0) || (errcheck && ret == -EDEADLK);
		if (!done)
			mutex_priority_inherit(current, m);
		sched_unlock();
		done;
	}));

	if (wait_ret != 0) {
		ret = wait_ret;
	}

	return ret;
}

static inline int mutex_this_owner(struct mutex *m) {
	return m->holder == schedee_get_current();
}

int mutex_trylock(struct mutex *m) {
	int res;
	struct schedee *current = schedee_get_current();

	assert(m);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if (mutex_is_static_inited(m))
		mutex_complete_static_init(m);

	sched_lock();
	{
		if (m->attr.type == MUTEX_ERRORCHECK) {
			if (!mutex_this_owner(m)) {
				res = mutex_trylock_schedee(current, m);
			} else {
				res = -EDEADLK;
			}
		} else if (m->attr.type == MUTEX_RECURSIVE) {
			if (mutex_this_owner(m)) {
				++m->lock_count;
				res = 0;
			} else {
				res = mutex_trylock_schedee(current, m);
			}
		} else {
			res = mutex_trylock_schedee(current, m);
		}
	}
	sched_unlock();
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));
	return res;
}

int mutex_unlock(struct mutex *m) {
	int res;
	struct schedee *current = schedee_get_current();

	assert(m);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	res = 0;
	sched_lock();
	{
		if (m->attr.type == MUTEX_ERRORCHECK) {
			if (mutex_this_owner(m)) {
				mutex_unlock_schedee(current, m);
			} else {
				res = -EPERM;
			}
		} else if (m->attr.type == MUTEX_RECURSIVE) {
			if (mutex_this_owner(m)) {
				assert(m->lock_count > 0);
				if (--m->lock_count == 0) {
					mutex_unlock_schedee(current, m);
				}
			} else {
				res = -EPERM;
			}
		} else {
			mutex_unlock_schedee(current, m);
		}
	}
	sched_unlock();
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));
	return res;
}
