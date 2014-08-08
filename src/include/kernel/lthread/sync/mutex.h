/**
 * @file
 * @brief
 *
 * @date 7.08.2014
 * @author Vita Loginova
 */

#ifndef KERNEL_LTHREAD_SYNC_MUTEX_H_
#define KERNEL_LTHREAD_SYNC_MUTEX_H_

#include <kernel/lthread/waitq.h>

#define mutex_lock_lthread(mutex) \
	WAITQ_WAIT_LTHREAD(&(mutex)->wq, ({ \
		int done; \
		done = (mutex_trylock_schedee(mutex) == 0); \
		if (!done) \
			priority_inherit(schedee_get_current(), mutex); \
		done; \
	}))

#endif /* KERNEL_LTHREAD_SYNC_MUTEX_H_ */
