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

#include <lib/list.h>
#include <kernel/thread/api.h>

/**
 * Defines Mutex structure.
 */
struct mutex {
	struct event event;
	/** Begin of the list for condition variable. */
	struct list_head *sleeped_thread_list;
	int lockscount;
};

/**
 * initializes given mutex
 * @param mutex mutex to initialize
 */
extern void mutex_init(struct mutex *mutex);

/**
 * Locks the mutex, binds it to the current thread and increases lockscount.
 * If the mutex have been already locked by another thread,
 * puts the current thread to a waiting state, trying to lock again when active.
 * If the mutex have been locked by the same thread then just increases lockscount.
 *
 * @param free_mutex Mutex to lock.
 */
extern void mutex_lock(struct mutex *free_mutex);

/**
 * Unleashes the mutex from lock and unbinds it, if lockscount
 * is equal to zero. Otherwise just decreases lockscount.
 *
 * @param locked_mutex Previously locked mutex.
 */
extern void mutex_unlock(struct mutex *locked_mutex);

/**
 * Tries to lock the mutex.
 * Returns 0 if the mutex was locked successfully. Otherwise returns error code.
 *
 * @param free_mutex Mutex to lock.
 */
extern int mutex_trylock(struct mutex *free_mutex);

#endif /* MUTEX_H_ */
