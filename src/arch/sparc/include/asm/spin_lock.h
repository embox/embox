/**
 * @spin_lock.h
 *
 * @date 19.11.2009
 * @author Anton Bondarev
 */

#ifndef SPIN_LOCK_H_
#define SPIN_LOCK_H_

#include <kernel/irq.h>

//FIXME spin_lock has to have different signature
# if 0
typedef __spin_lock_t spin_lock_t;
#define SPIN_LOCK_INITIALIZER   __SPIN_LOCK_INITIALIZER

#define spin_lock_init(lock)    __spin_lock_init (lock)
#define spin_lock(lock)         __spin_lock (lock)
#define spin_try_lock(lock)     __spin_try_lock (lock)
#define spin_unlock(lock)       __spin_unlock (lock)
#define spin_lock_locked(lock)  __spin_lock_locked (lock)
#endif

/**
 * now we have single thread system therefore we can use local_irq_save
 * instead of spin_lock
 */
#define spin_lock() local_irq_save()
#define spin_unlock(spin) local_irq_restore(spin)

#endif /* SPIN_LOCK_H_ */
