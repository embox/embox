/**
 * @file
 * @brief Implements mutex methods.
 *
 * @date 05.05.2010
 * @author Nikolay Malkovsky
 * @author Skorodumov Kirill
 */

#include <assert.h>
#include <errno.h>

#include <util/math.h>
#include <lib/list.h>
#include <hal/ipl.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/thread/sched.h>

//static void mutex_priority_inherit(struct mutex *m);
//static void mutex_priority_uninherit(struct mutex *m);

void mutex_init(struct mutex *m) {
	event_init(&m->event, "mutex");
	m->lockscount = 0;
	m->mutex_prioryty_max = 0;
}

void mutex_lock(struct mutex *m) {
	struct thread *current;
	assert(critical_allows(CRITICAL_PREEMPT));

	sched_lock();

	current = thread_self();

	m->mutex_prioryty_max = max(m->mutex_prioryty_max, current->priority);

	if (m->lockscount == 0) {
		m->hendler = current;
		m->lockscount++;
		sched_unlock();
		return;
	}

	if (m->hendler == current) {
		m->lockscount++;
		sched_unlock();
		return;
	}

	sched_sleep_locked(&m->event);
//	mutex_priority_inherit(m);

	sched_unlock();
}

void mutex_unlock(struct mutex *m) {
	sched_lock();

	m->lockscount--;

	if (m->lockscount != 0) {
		sched_unlock();
		return;
	}

//	mutex_priority_uninherit(m);
	sched_wake_one(&m->event);

	sched_unlock();
}

int mutex_trylock(struct mutex *m) {
	sched_lock();

	if (m->lockscount == 0) {
		m->lockscount++;
		return 0;
	} else {
		return -EAGAIN;
	}

	sched_unlock();
}

#if 0
static void mutex_priority_inherit(struct mutex *m) {
	if (list_empty(&m->event.sleep_queue)) {
		return;
	}

	sched_change_scheduling_priority(thread_self(), m->mutex_prioryty_max);
}

static void mutex_priority_uninherit(struct mutex *m) {
	struct thread *current = thread_self();

	sched_set_priority(current, current->initial_priority);
}
#endif
