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

static int mutex_priority_inherit(struct mutex *m);
static void mutex_priority_uninherit(struct mutex *m);

void mutex_init(struct mutex *m) {
	event_init(&m->event, "mutex");
	m->lock_scount = 0;
	m->priority_max = 0;
	m->holder = NULL;
}

void mutex_lock(struct mutex *m) {
	struct thread *current;
	assert(critical_allows(CRITICAL_PREEMPT));

	sched_lock();

	current = sched_current();

	m->priority_max = max(m->priority_max, current->priority);

	if (m->holder == NULL || m->holder == current) {
		m->holder = current;
		m->lock_scount++;
		sched_unlock();
		return;
	}

	sched_sleep_locked(&m->event);
	mutex_priority_inherit(m);

	sched_unlock();
}

void mutex_unlock(struct mutex *m) {
	sched_lock();

	m->lock_scount--;

	if (m->lock_scount != 0) {
		sched_unlock();
		return;
	}

	mutex_priority_uninherit(m);
	m->holder = NULL;
	sched_wake_one(&m->event);

	sched_unlock();
}

int mutex_trylock(struct mutex *m) {
	sched_lock();

	if (m->lock_scount == 0) {
		m->lock_scount++;
		return 0;
	} else {
		return -EAGAIN;
	}

	sched_unlock();
}

static int mutex_priority_inherit(struct mutex *m) {
	struct thread *waiter;

	if (list_empty(&m->event.sleep_queue)) {
		return 0;
	}

	list_for_each_entry(waiter, &m->event.sleep_queue, sched_list) {

		if (m->holder == waiter) {
			return -EDEADLOCK;
		}

		if (m->priority_max >= waiter->priority) {
			sched_change_scheduling_priority(waiter, m->priority_max);
		}

		if (m->holder->initial_priority > waiter->priority) {
			sched_change_scheduling_priority(m->holder, m->priority_max);
		}
	}

	return 0;
}

static void mutex_priority_uninherit(struct mutex *m) {
	sched_set_priority(m->holder, (m->holder)->initial_priority);
}
