/**
 * @spin_lock.h
 *
 * @date 19.11.2009
 * @author Anton Bondarev
 */

#ifndef SPIN_LOCK_H_
#define SPIN_LOCK_H_

/**
 * now we have single thread system therefore we can use local_irq_save instead of spin_lock
 */
#define spin_lock() local_irq_save()
#define spin_unlock(spin) local_irq_restore(spin)

#endif /* SPIN_LOCK_H_ */
