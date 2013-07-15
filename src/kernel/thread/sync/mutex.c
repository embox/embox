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

static int priority_inherit(struct thread *t);
static void priority_uninherit(struct thread *t);

void mutex_init(struct mutex *m) {
	wait_queue_init(&m->wq);
	m->lock_count = 0;
	m->holder = NULL;
#if 0
	m->priority = THREAD_PRIORITY_MIN;
#endif
}

void mutex_lock(struct mutex *m) {
	struct thread *current = thread_self();

	assert(m);
	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		current->mutex_waiting = m;

		while (trylock_sched_locked(m, current) != 0) {
			/* We have to wait for a mutex to be released. */

			priority_inherit(current);
			wait_queue_wait_locked(&m->wq, SCHED_TIMEOUT_INFINITE); /* Sleep here... */
		}

		current->mutex_waiting = NULL;
	}
	sched_unlock();
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
#if 0
	m->priority = current->priority;
#endif

	return 0;
}

void mutex_unlock(struct mutex *m) {
	struct thread *current = thread_self();

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
		wait_queue_notify(&m->wq);
	}
	out: sched_unlock();
}

static int priority_inherit(struct thread *t) {
	struct mutex *m = t->mutex_waiting;
	assert(critical_inside(CRITICAL_SCHED_LOCK));

	if (m->holder->sched_priority >= t->sched_priority) {
		return 0;
	}

	sched_change_scheduling_priority(m->holder, t->sched_priority);

	return 0;
}

static void priority_uninherit(struct thread *t) {
	assert(critical_inside(CRITICAL_SCHED_LOCK));
	sched_change_scheduling_priority(t, t->initial_priority);
}
