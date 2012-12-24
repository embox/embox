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

#include <lib/list.h>
#include <hal/ipl.h>
#include <kernel/thread/state.h>
#include <kernel/thread.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/thread/sched.h>
#include <math.h>

static int trylock_sched_locked(struct mutex *m, struct thread *current);

static int priority_inherit(struct thread *t);
static void priority_uninherit(struct thread *t);

void mutex_init(struct mutex *m) {
	sleepq_init(&m->sq);
	m->lock_count = 0;
	m->holder = NULL;
	m->priority = THREAD_PRIORITY_MIN;
}

void mutex_lock(struct mutex *m) {
	struct thread *current = sched_current();

	assert(m);
	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		current->mutex_waiting = m;

		while (trylock_sched_locked(m, current) != 0) {
			/* We have to wait for a mutex to be released. */

			priority_inherit(current);
			sched_sleep_locked(&m->sq, SCHED_TIMEOUT_INFINITE); /* Sleep here... */
		}

		current->mutex_waiting = NULL;
	}
	sched_unlock();
}

int mutex_trylock(struct mutex *m) {
	int ret;
	struct thread *current = sched_current();

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

	if (m->holder == current) {
		/* Nested locks. */
		m->lock_count++;
		return 0;
	}

	if (m->holder) {
		return -EAGAIN;
	}

	m->lock_count = 1;
	m->holder = current;
	m->priority = current->priority;

	return 0;
}

void mutex_unlock(struct mutex *m) {
	struct thread *current = sched_current();

	assert(m);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	sched_lock();
	{
		assert(m->holder == current);
		assert(m->lock_count > 0);

		if(--m->lock_count != 0) {
			goto out;
		}

		priority_uninherit(current);

		m->holder = NULL;
		sched_wake_one(&m->sq);
	}
	out: sched_unlock();
}

static int priority_inherit(struct thread *t) {
	struct mutex *m = t->mutex_waiting;
	assert(critical_inside(CRITICAL_SCHED_LOCK));

	if (m->holder->priority >= t->priority) {
		return 0;
	}

	sched_change_scheduling_priority(m->holder, t->priority);

	return 0;
}

static void priority_uninherit(struct thread *t) {
	assert(critical_inside(CRITICAL_SCHED_LOCK));
	sched_change_scheduling_priority(t, t->initial_priority);
}
