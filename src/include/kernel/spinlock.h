/**
 * @file
 * @brief
 *
 * @date 08.02.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 */

#ifndef KERNEL_SPINLOCK_H_
#define KERNEL_SPINLOCK_H_

#include <linux/compiler.h>
#include <module/embox/arch/libarch.h>
#include <sys/types.h>

#define SPIN_UNLOCKED 0
#define SPIN_LOCKED   1

/**
 * spin_trylock -- try to lock object without waiting
 * @param lock  object to lock
 * @retval      1 if successfully blocked otherwise 1
 */
static inline int spin_trylock(spinlock_t *lock) {
#ifdef __HAVE_ARCH_CMPXCHG
	return SPIN_UNLOCKED == cmpxchg(lock, SPIN_UNLOCKED, SPIN_LOCKED);
#else /* !__HAVE_ARCH_CMPXCHG */
#warning "cmpxchg arch operation required"
	return 0;
#endif /* __HAVE_ARCH_CMPXCHG */
}

/**
 * spin_lock -- try to lock object or wait until it's will done
 * @param lock  object to lock
 */
static inline void spin_lock(spinlock_t *lock) {
	while (!spin_trylock(lock)) { }
}

/**
 * spin_unlock -- unlock blocked object
 * @param lock  object to unlock
 */
static inline void spin_unlock(spinlock_t *lock) {
	*lock = SPIN_UNLOCKED;
	__barrier();
}

#endif /* !KERNEL_SPINLOCK_H_ */
