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

#include <lib/list.h>
#include <hal/ipl.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/thread/sched.h>

struct mutex *idle_mutex;

void mutex_init(struct mutex *mutex) {
	event_init(&mutex->event, "mutex");
	mutex->lockscount = 0;
}

void mutex_lock(struct mutex *mutex) {
	assert(critical_allows(CRITICAL_PREEMPT));

	sched_lock();

	if (mutex->lockscount == 0) {
		mutex->lockscount++;
	} else {
		sched_sleep_locked(&mutex->event);
	}

	sched_unlock();
}

void mutex_unlock(struct mutex *mutex) {
	if (list_empty(&mutex->event.sleep_queue)) {
		mutex->lockscount--;
	} else {
		sched_wake_one(&mutex->event);
	}
}

int mutex_trylock(struct mutex *mutex) {
	sched_lock();
	if (mutex->lockscount == 0) {
		mutex->lockscount++;
		return 0;
	} else {
		return -EAGAIN;
	}
	sched_unlock();
}
