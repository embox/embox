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

struct mutex *idle_mutex;

static void mutex_priority_inherit(struct mutex *m);
static void mutex_priority_uninherit(struct mutex *m);

void mutex_init(struct mutex *m) {
	event_init(&m->event, "mutex");
	m->lockscount = 0;
}

void mutex_lock(struct mutex *m) {
	assert(critical_allows(CRITICAL_PREEMPT));

	sched_lock();

	if (m->lockscount == 0) {
		m->lockscount++;
	} else {
		sched_sleep_locked(&m->event);
		mutex_priority_inherit(m);
	}

	sched_unlock();
}

void mutex_unlock(struct mutex *m) {
	sched_lock();

	if (list_empty(&m->event.sleep_queue)) {
		m->lockscount--;
	} else {
		mutex_priority_uninherit(m);
		sched_wake_one(&m->event);
	}

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

static void mutex_priority_inherit(struct mutex *m) {
	struct thread *ptr;
	thread_priority_t new = 0;

	if (list_empty(&m->event.sleep_queue)) {
		return;
	}

	list_for_each_entry(ptr, &m->event.sleep_queue, sched_list) {
		new = max(new, ptr->priority);
	}

	if (new < sched_current()->priority) {
		return;
	}

	sched_set_priority(sched_current(), new);
}

static void mutex_priority_uninherit(struct mutex *m) {
	sched_set_priority(sched_current(), sched_current()->initial_priority);
}

#if 0
#endif
