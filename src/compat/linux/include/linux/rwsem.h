/**
 * @file
 * @brief
 *
 * @date 01.08.2013
 * @author Andrey Gazukin
 */

#ifndef __LINUX_RWSEM_H__
#define __LINUX_RWSEM_H__

#include <asm/semaphore.h>

struct rw_semaphore;

#define down_read(sem) mutex_lock((mutex_t *)sem)
#define down_read_trylock(sem) mutex_trylock((mutex_t *)sem)
#define down_write(sem) mutex_lock((mutex_t *)sem)
#define down_write_trylock(sem) mutex_trylock((mutex_t *)sem)
#define up_read(sem) mutex_unlock((mutex_t *)sem)
#define up_write(sem) mutex_unlock((mutex_t *)sem)
#define downgrade_write(sem)

#endif // __LINUX_RWSEM_H__
