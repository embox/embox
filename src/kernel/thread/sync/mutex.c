/**
 * @file
 * @brief Implements mutex methods.
 *
 * @date 05.05.2010
 * @author Nikolay Malkovsky
 * @author Skorodumov Kirill
 */

#include <errno.h>
#include <lib/list.h>
#include <hal/ipl.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/thread/sched.h>

struct mutex *idle_mutex;

void mutex_init(struct mutex *mutex) {
	event_init(&mutex->event);
	mutex->lockscount = 0;
}

void mutex_lock(struct mutex *mutex) {
	sched_lock();
	if (mutex->lockscount == 0) {
		mutex->lockscount++;
	} else {
		scher_sleep(&mutex->event);
	}
	sched_unlock();
}

void mutex_unlock(struct mutex *mutex) {
	if (list_empty(&mutex->event.threads_list)) {
		mutex->lockscount--;
	} else {
		sched_wakeup_first(&mutex->event);
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
