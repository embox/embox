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
 * Initializes given @p mutex with default type.
 *
 * @param mutex Mutex to initialize.
 */
extern void mutex_init_schedee(struct mutex *mutex);

/**
 * Unleashes the mutex from lock and unbinds it.
 *
 * @param self Current schedee holding @p mutex.
 * @param mutex Previously locked mutex.
 */
extern void mutex_unlock_schedee(struct schedee *self, struct mutex *mutex);

/**
 * Tries to lock the mutex.
 *
 * @param fself Current schedee to hold @p mutex.
 * @param mutex Mutex to lock.
 *
 * @return Error code.
 * @retval 0
 *   @p mutex successfully locked.
 * @retval -EBUSY
 *   @p mutex is already held by some schedee.
 */
extern int mutex_trylock_schedee(struct schedee *self, struct mutex *mutex);

/**
 * Inherits priority in order to prevent the priority inversion.
 *
 * @param self Current schedee trying to lock @p mutex.
 * @param mutex The mutex which holder should inherit priority.
 */
extern void mutex_priority_inherit(struct schedee *self, struct mutex *mutex);

/**
 * Uninherits priority after mutex is unlocked.
 *
 * @param self Current schedee which priority to uninherit.
 */
extern void mutex_priority_uninherit(struct schedee *self);


#endif /* KERNEL_SCHEDEE_SYNC_MUTEX_H_ */
