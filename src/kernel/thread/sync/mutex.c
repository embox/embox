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
	scheduler_lock();
	if (mutex->lockscount == 0) {
		mutex->lockscount++;
	} else {
		scheduler_sleep(&mutex->event);
	}
	scheduler_unlock();
}

void mutex_unlock(struct mutex *mutex) {
	if (list_empty(&mutex->event.threads_list)) {
		mutex->lockscount--;
	} else {
		scheduler_wakeup_first(&mutex->event);
	}
}

int mutex_trylock(struct mutex *mutex) {
	scheduler_lock();
	if (mutex->lockscount == 0) {
		mutex->lockscount++;
		return 0;
	} else {
		return -EAGAIN;
	}
	scheduler_unlock();
}
