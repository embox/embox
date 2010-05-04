/**
 * @file
 * @brief Defines mutex structure and methods associated with it.
 * Implemented in src/kernel/mutex.c .
 *
 * @date 05.05.2010
 * @author Nikolay Malkovsky
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#include <kernel/thread.h>

/**
 * Defines Mutex structure.
 */
struct mutex {
	struct thread *bound_thread;
	/* struct list_head *locked_thread_list;*/
	int lockscount;
};

/**
 * Locks the mutex, binds it to the current thread and increases lockscount.
 * If the mutex have been already locked by another thread,
 * puts the current thread to a waiting state, trying to lock again when active.
 * If the mutex have been locked by the same thread then just increases lockscount.
 *
 * @param free_mutex Mutex to lock.
 */
void mutex_lock(struct mutex *free_mutex);

/**
 * Unleashes the mutex from lock and unbinds it, if lockscount
 * is equal to zero. Otherwise just decreases lockscount.
 *
 * @param locked_mutex Previously locked mutex.
 */
void mutex_unlock(struct mutex *locked_mutex);

/**
 * Tries to lock the mutex.
 * Returns 0 if the mutex was locked successfully. Otherwise returns error code.
 *
 * @param free_mutex Mutex to lock.
 */
int mutex_trylock(struct mutex *free_mutex);

#endif /* MUTEX_H_ */
