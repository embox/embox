/**
 * @file
 * @brief TODO
 *
 * @date 15.02.10
 * @author Eldar Abusalimov
 */

#ifndef LINUX_SPINLOCK_H_
#define LINUX_SPINLOCK_H_

#include <asm/system.h>

/* Used to suppress 'defined but not used' warning for lock argument. */
#define __lock_do_once(lock, stmts) \
	do { stmts } while ((typeof(lock)) 0)

#define __lock_do_nothing(lock) __lock_do_once(lock,)

#define spin_lock(lock)  __lock_do_nothing(lock)
#define read_lock(lock)  __lock_do_nothing(lock)
#define write_lock(lock) __lock_do_nothing(lock)

#define spin_unlock(lock)  __lock_do_nothing(lock)
#define read_unlock(lock)  __lock_do_nothing(lock)
#define write_unlock(lock) __lock_do_nothing(lock)

#define spin_lock_irq(lock)  __lock_do_once(lock,local_irq_disable();)
#define read_lock_irq(lock)  __lock_do_once(lock,local_irq_disable();)
#define write_lock_irq(lock) __lock_do_once(lock,local_irq_disable();)

#define spin_unlock_irq(lock)  __lock_do_once(lock,local_irq_enable();)
#define read_unlock_irq(lock)  __lock_do_once(lock,local_irq_enable();)
#define write_unlock_irq(lock) __lock_do_once(lock,local_irq_enable();)

#define spin_unlock_irqrestore(lock, flags)  __lock_do_once(lock,local_irq_restore(flags);)
#define read_unlock_irqrestore(lock, flags)  __lock_do_once(lock,local_irq_restore(flags);)
#define write_unlock_irqrestore(lock, flags) __lock_do_once(lock,local_irq_restore(flags);)

#define spin_lock_irqsave(lock, flags)  __lock_do_once(lock,local_irq_save(flags);)
#define read_lock_irqsave(lock, flags)  __lock_do_once(lock,local_irq_save(flags);)
#define write_lock_irqsave(lock, flags) __lock_do_once(lock,local_irq_save(flags);)

#define SPIN_LOCK_UNLOCKED 0

#define DEFINE_SPINLOCK(x) spinlock_t x = SPIN_LOCK_UNLOCKED

#endif /* LINUX_SPINLOCK_H_ */
