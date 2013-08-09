/**
 * @file
 * @brief
 *
 * @date 29.07.2013
 * @author Andrey Gazukin
 */

#ifndef __ASM_SEMAPHORE_H__
#define __ASM_SEMAPHORE_H__

#include <kernel/thread/sync/mutex.h>

struct semaphore {
	struct mutex x;
};

#define DECLARE_MUTEX(x) struct semaphore x = { { 0 } };
#define DECLARE_MUTEX_LOCKED(x) struct semaphore x = { { 1 } };

#define init_MUTEX(sem) mutex_init((struct mutex *)sem)
#define init_MUTEX_LOCKED(sem) do { mutex_init((struct mutex *)sem);\
									mutex_lock((struct mutex *)sem); } while(0)
#define down(sem) mutex_lock((struct mutex *)sem)
#define down_interruptible(sem) ({ mutex_lock((struct mutex *)sem), 0; })
#define down_trylock(sem) mutex_trylock((struct mutex *)sem)
#define up(sem) mutex_unlock((struct mutex *)sem)

#endif /* __ASM_SEMAPHORE_H__ */
