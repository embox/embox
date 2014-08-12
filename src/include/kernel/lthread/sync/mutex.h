/**
 * @file
 * @brief Defines lthread-specific methods associated with mutex. You can also
 * or instead use ones from schedee/sync/mutex.h
 *
 * @date 7.08.2014
 * @author Vita Loginova
 */

#ifndef KERNEL_LTHREAD_SYNC_MUTEX_H_
#define KERNEL_LTHREAD_SYNC_MUTEX_H_

#include <kernel/lthread/waitq.h>

/**
 * Tries to lock the mutex.
 *
 * @param mutex mutex to be locked
 *
 * @return waiting result
 * @retval 0 mutex locked
 * @retval -EAGAIN lthread placed in mutex->wq, lthread has to finish its routine
 *                 in order to be waken up by mutex->wq when other schedee frees
 *                 mutex.
 */
#define mutex_trylock_lthread(mutex) \
	WAITQ_WAIT_LTHREAD(&(mutex)->wq, ({ \
		int done; \
		done = (mutex_trylock_schedee(mutex) == 0); \
		if (!done) \
			priority_inherit(schedee_get_current(), mutex); \
		done; \
	}))

#endif /* KERNEL_LTHREAD_SYNC_MUTEX_H_ */
