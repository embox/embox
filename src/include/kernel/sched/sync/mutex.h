/**
 * @file
 * @brief Defines mutex structure and methods associated with it.
 *
 * @date 7.08.2014
 * @author Vita Loginova
 */

#ifndef KERNEL_SCHEDEE_SYNC_MUTEX_H_
#define KERNEL_SCHEDEE_SYNC_MUTEX_H_

#include <kernel/sched/sync/mutexattr.h>
#include <kernel/sched/waitq.h>

struct thread;

/**
 * Defines Mutex structure.
 */
struct mutex {
	struct waitq wq;
	struct schedee *holder;
	struct mutexattr attr;

	int lock_count;
};

/**
 * Initializes given mutex, mutex type is default.
 *
 * @param mutex mutex to initialize
 */
extern void mutex_init_schedee(struct mutex *m);

/**
 * Unleashes the mutex from lock and unbinds it.
 *
 * @param locked_mutex Previously locked mutex.
 */
extern void mutex_unlock_schedee(struct mutex *locked_mutex);

/**
 * Tries to lock the mutex.
 * Returns 0 if the mutex was locked successfully. Otherwise returns error code.
 *
 * @param free_mutex Mutex to lock.
 */
extern int mutex_trylock_schedee(struct mutex *free_mutex);

extern void priority_inherit(struct schedee *t, struct mutex *m);
extern void priority_uninherit(struct schedee *t);


#endif /* KERNEL_SCHEDEE_SYNC_MUTEX_H_ */
