/**
 * @file
 * @brief Implements mutex methods.
 *
 * @date 05.05.2010
 * @author Nikolay Malkovsky
 */

#include <errno.h>
#include <lib/list.h>
#include <hal/ipl.h>
#include <kernel/thread.h>
#include <kernel/mutex.h>
#include <kernel/scheduler.h>

struct mutex *idle_mutex;

void mutex_lock(struct mutex *free_mutex) {
	scheduler_lock();
	if (free_mutex->bound_thread == NULL || free_mutex->bound_thread
			== current_thread) {
		++free_mutex->lockscount;
		free_mutex->bound_thread = current_thread;
		scheduler_unlock();
		return;
	}
	free_mutex->bound_thread->state = THREAD_STATE_WAIT;
	list_add_tail(current_thread, free_mutex->locked_thread_list);
	scheduler_unlock();
	/* if the current thread reaches his time limit before calling scheduler_dispatch()
	 * then scheduler_dispatch() will cause the current thread to loose some of its time
	 * when becomes active.
	 */
	scheduler_dispatch();
}

void mutex_unlock(struct mutex *locked_mutex) {
	if (locked_mutex->bound_thread != current_thread) {
		--locked_mutex->lockscount;
		if (locked_mutex->lockscount == 0) {
			scheduler_lock();
			struct thread* locked_thread
				= list_entry(locked_mutex, struct mutex, locked_thread_list);
			locked_thread->state = THREAD_STATE_RUN;
			/*TODO
			 There must be no interruptions to prevent this thread from locking again by
			 mutex_lock(). It may create some trouble.
			 */
			list_del(&locked_thread);
			locked_mutex->bound_thread = NULL;
			scheduler_unlock();
		}
	}
}

int mutex_trylock(struct mutex *free_mutex) {
	scheduler_lock();
	if (free_mutex->bound_thread == NULL || free_mutex->bound_thread
			== current_thread) {
		++free_mutex->lockscount;
		free_mutex->bound_thread = current_thread;
		scheduler_unlock();
		return 0;
	}
	scheduler_unlock();
	return -EAGAIN;
}
