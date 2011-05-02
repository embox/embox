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
#include <string.h>
#include <lib/list.h>
#include <hal/ipl.h>
#include <kernel/thread/state.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/thread/sched.h>

static int mutex_priority_inherit(struct mutex *m);
static void mutex_priority_uninherit(struct mutex *m);

void mutex_init(struct mutex *m) {
	event_init(&m->event, "mutex");
	m->lock_count = 0;
	m->priority = 0;
	m->holder = NULL;
}

void mutex_lock(struct mutex *m) {
	struct thread *current;
	assert(critical_allows(CRITICAL_PREEMPT));

	sched_lock();

	current = sched_current();

	m->priority = max(m->priority, current->priority);

	if (m->holder == NULL || m->holder == current) {
		m->holder = current;
		m->lock_count++;
		sched_unlock();
		return;
	}

	sched_sleep_locked(&m->event);
	mutex_priority_inherit(m);

	sched_unlock();
}

void mutex_unlock(struct mutex *m) {
	sched_lock();

	m->lock_count--;

	if (m->lock_count != 0) {
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

	if (m->lock_count == 0) {
		m->lock_count++;
		return 0;
	} else {
		return -EAGAIN;
	}

	sched_unlock();
}

static int mutex_priority_inherit(struct mutex *m) {
	struct thread *waiter;
	struct event *e;

	if (list_empty(&m->event.sleep_queue)) {
		return 0;
	}

	waiter = list_entry(&m->event.sleep_queue, struct thread, sched_list);

	if (waiter == m->holder) {
		return -EDEADLOCK;
	}

	if (m->holder->priority >= waiter->priority) {
		return 0;
	}

	sched_change_scheduling_priority(m->holder, waiter->priority);

	if (thread_state_sleeping(m->holder->state)) {
		e = list_entry(&m->holder->sched_list, struct event, sleep_queue);

		if (strcmp(e->name, "mutex")) {
			return 0;
		}

		mutex_priority_inherit(list_entry(e, struct mutex, event));
	}

	return 0;
}

static void mutex_priority_uninherit(struct mutex *m) {
	sched_set_priority(m->holder, (m->holder)->initial_priority);
}
