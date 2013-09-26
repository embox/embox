/**
 * @file
 * @brief Defines mutex structure and methods associated with it.
 *
 * @date 05.05.10
 * @author Nikolay Malkovsky
 */

#ifndef KERNEL_THREAD_SYNC_MUTEX_H_
#define KERNEL_THREAD_SYNC_MUTEX_H_

#include <sys/cdefs.h>
#include <kernel/sched/wait_queue.h>
#include <kernel/thread/sync/mutexattr.h>

struct thread;

/**
 * Defines Mutex structure.
 */
struct mutex {
	struct wait_queue wq;
	struct thread *holder;
	struct mutexattr attr;

	int lock_count;
};

#define MUTEX_INIT_STATIC \
	{ \
		{ /*wait_queue init */ \
			{/* dlist_init*/ NULL, NULL, NULL,}, \
			/* flags */0, \
			}, \
		/* holder*/ NULL, \
		{ /*mutexattr init */ \
			/* type */ MUTEX_DEFAULT, \
			}, \
		/* lock_couunt */ 0 \
	}

#define MUTEX_INIT(m)  {.wq=WAIT_QUEUE_INIT(m.wq), .holder=NULL, .lock_count=0}

__BEGIN_DECLS

/**
 * initializes given mutex with attribute
 * @param m mutex to initialize
 * @param attr mutex attribute
 */
extern void mutex_init_default(struct mutex *m, const struct mutexattr *attr);

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
extern int mutex_lock(struct mutex *free_mutex);

/**
 * Unleashes the mutex from lock and unbinds it, if lockscount
 * is equal to zero. Otherwise just decreases lockscount.
 *
 * @param locked_mutex Previously locked mutex.
 */
extern int mutex_unlock(struct mutex *locked_mutex);

/**
 * Tries to lock the mutex.
 * Returns 0 if the mutex was locked successfully. Otherwise returns error code.
 *
 * @param free_mutex Mutex to lock.
 */
extern int mutex_trylock(struct mutex *free_mutex);

__END_DECLS

#endif /* KERNEL_THREAD_SYNC_MUTEX_H_ */
