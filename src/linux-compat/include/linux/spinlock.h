/**
 * @file
 * @brief TODO
 *
 * @date 15.02.2010
 * @author Eldar Abusalimov
 */

#ifndef LINUX_SPINLOCK_H_
#define LINUX_SPINLOCK_H_

#include <asm/system.h>

#define spin_lock(lock)  do { } while(0)
#define read_lock(lock)  do { } while(0)
#define write_lock(lock) do { } while(0)

#define spin_unlock(lock)  do { } while(0)
#define read_unlock(lock)  do { } while(0)
#define write_unlock(lock) do { } while(0)

#define spin_lock_irq(lock)  local_irq_disable();
#define read_lock_irq(lock)  local_irq_disable();
#define write_lock_irq(lock) local_irq_disable();

#define spin_unlock_irq(lock)  local_irq_enable()
#define read_unlock_irq(lock)  local_irq_enable()
#define write_unlock_irq(lock) local_irq_enable()

#define spin_unlock_irqrestore(lock, flags)  local_irq_restore(flags)
#define read_unlock_irqrestore(lock, flags)  local_irq_restore(flags)
#define write_unlock_irqrestore(lock, flags) local_irq_restore(flags)

#define spin_lock_irqsave(lock, flags)  local_irq_save(flags)
#define read_lock_irqsave(lock, flags)  local_irq_save(flags)
#define write_lock_irqsave(lock, flags) local_irq_save(flags)

#endif /* LINUX_SPINLOCK_H_ */
