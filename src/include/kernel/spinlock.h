/**
 * @file
 * @brief
 *
 * @date 08.02.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_SPINLOCK_H_
#define KERNEL_SPINLOCK_H_

#include <linux/compiler.h>
#include <module/embox/arch/libarch.h>
#include <sys/types.h>

#define SPIN_UNLOCKED 0
#define SPIN_LOCKED   1

static inline int spin_trylock(spinlock_t *lock) {
#ifdef __HAVE_ARCH_CMPXCHG
	return SPIN_UNLOCKED == cmpxchg(lock, SPIN_UNLOCKED, SPIN_LOCKED);
#else /* !__HAVE_ARCH_CMPXCHG */
#warning "cmpxchg arch operation required"
	return 0;
#endif /* __HAVE_ARCH_CMPXCHG */
}

static inline void spin_lock(spinlock_t *lock) {
	while (!spin_trylock(lock)) { }
}

static inline void spin_unlock(spinlock_t *lock) {
	*lock = SPIN_UNLOCKED;
	__barrier();
}

#endif /* !KERNEL_SPINLOCK_H_ */
