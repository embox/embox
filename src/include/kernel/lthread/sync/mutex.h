/**
 * @file
 * @brief Defines lthread-specific methods associated with mutex. You can also
 * or instead use ones from sched/sync/mutex.h
 *
 * @date 7.08.2014
 * @author Vita Loginova
 */

#ifndef KERNEL_LTHREAD_SYNC_MUTEX_H_
#define KERNEL_LTHREAD_SYNC_MUTEX_H_

#include <kernel/lthread/waitq.h>

/**
 * Tries to lock @p mutex.
 *
 * @param self Current lthread to hold @p mutex.
 * @param mutex Mutex to lock.
 *
 * @return Error code.
 * @retval 0
 *   @p mutex successfully locked.
 * @retval -EAGAIN
 *   lthread placed in mutex->wq, lthread has to finish its routine in order
 *   to be waken up by mutex->wq when other schedee frees mutex.
 */
static inline int mutex_trylock_lthread(struct lthread *self,
					struct mutex *mutex) {
	return WAITQ_WAIT_LTHREAD(self, &mutex->wq, ({
			int done;
			done = (mutex_trylock_schedee(&self->schedee, mutex) == 0);
			if (!done) {
				mutex_priority_inherit(&self->schedee, mutex);
			}
			done;
		}));
}

/**
 * Unleashes @p mutex from lock and unbinds it.
 *
 * @param self Current lthread holding @p mutex.
 * @param mutex Previously locked mutex.
 */
static inline void mutex_unlock_lthread(struct lthread *self,
					struct mutex *mutex) {
	mutex_unlock_schedee(&self->schedee, mutex);
}

#endif /* KERNEL_LTHREAD_SYNC_MUTEX_H_ */
