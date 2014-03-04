/**
 * @file
 * @brief TODO
 *
 * @date 15.02.10
 * @author Eldar Abusalimov
 */

#ifndef LINUX_SPINLOCK_H_
#define LINUX_SPINLOCK_H_

#include <kernel/spinlock.h>

#define spin_lock_irq(lock)    spin_lock_ipl_disable(lock)
#define spin_unlock_irq(lock)  spin_unlock_ipl_enable(lock)

#define spin_lock_irqsave(lock, flags)      ({ flags = spin_lock_ipl(lock); })
#define spin_unlock_irqrestore(lock, flags)    spin_unlock_ipl(lock, flags)

#define DEFINE_SPINLOCK(x) \
	spinlock_t x = SPIN_STATIC_UNLOCKED

#endif /* LINUX_SPINLOCK_H_ */
