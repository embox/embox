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

static int mutex_priority_inherit(struct mutex *m, struct thread *t);
static void mutex_priority_uninherit(struct thread *t);

void mutex_init(struct mutex *m) {
	event_init(&m->event, "mutex");
	m->lock_count = 0;
	m->holder = NULL;
}

void mutex_lock(struct mutex *m) {
	struct thread *current;
	assert(critical_allows(CRITICAL_PREEMPT));

	sched_lock();

	current = sched_current();

	if (m->holder == NULL || m->holder == current) {
		m->holder = current;
		m->lock_count++;
		sched_unlock();
		return;
	}

	mutex_priority_inherit(m, sched_current());
	sched_sleep_locked(&m->event);

	sched_unlock();
}

void mutex_unlock(struct mutex *m) {
	sched_lock();

	m->lock_count--;

	if (m->lock_count != 0) {
		sched_unlock();
		return;
	}

	mutex_priority_uninherit(m->holder);
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

static int mutex_priority_inherit(struct mutex *m, struct thread *t) {
	struct event *e;

	if (t == m->holder) {
		return -EDEADLOCK;
	}

	if (m->holder->priority >= t->priority) {
		return 0;
	}

	sched_change_scheduling_priority(m->holder, t->priority);

	if (thread_state_sleeping(m->holder->state)) {
		e = list_entry(&m->holder->sched_list, struct event, sleep_queue);

		if (strcmp(e->name, "mutex")) {
			return 0;
		}

		mutex_priority_inherit(list_entry(e, struct mutex, event),
				list_entry(e, struct mutex, event)->holder);
	}

	return 0;
}

static void mutex_priority_uninherit(struct thread *t) {
	sched_set_priority(t, t->initial_priority);
}
