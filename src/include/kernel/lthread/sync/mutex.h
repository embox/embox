/**
 * @file
 * @brief
 *
 * @date 7.08.2014
 * @author Vita Loginova
 */

#ifndef KERNEL_LTHREAD_SYNC_MUTEX_H_
#define KERNEL_LTHREAD_SYNC_MUTEX_H_

#include <kernel/thread/sync/mutex.h>

#define mutex_lock_lthread(mutex) \
	WAITQ_WAIT_LTHREAD(&(mutex)->wq, mutex_trylock_schedee(mutex) == 0)

#endif /* KERNEL_LTHREAD_SYNC_MUTEX_H_ */
