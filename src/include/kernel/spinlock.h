/**
 * @file
 * @brief
 *
 * @date 08.02.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_SPINLOCK_H_
#define KERNEL_SPINLOCK_H_

#include <kernel/cpu.h>
#include <sys/types.h>
#include <module/embox/arch/smp.h>

//typedef unsigned long spinlock_t;

#define SPIN_UNLOCKED 0
#define SPIN_LOCKED   1

#define SPINLOCK_DEFINE(name) \
	spinlock_t name = SPIN_UNLOCKED

#define SPINLOCK_DECLARE(name) \
	extern spinlock_t name

#ifndef NOSMP

#define __barrier() \
	__asm__ __volatile__("" : : : "memory")

/* TODO: Replace it in arch. It is only for x86. */
static inline int spin_trylock(spinlock_t *lock) {
	unsigned long val = SPIN_LOCKED;

	__asm__ __volatile__ (
			"lock xchgb %0, %1"
			: "+r" (val), "+m" (*(lock))
	);

	__barrier();

	return val == SPIN_UNLOCKED;
}

static inline void spin_lock(spinlock_t *lock) {
	while (!spin_trylock(lock)) ;
}

static inline void spin_unlock(spinlock_t *lock) {
	*lock = SPIN_UNLOCKED;
	__barrier();
}

#else /* NOSMP */

#define spin_lock(lock)   do { (void) lock; } while (0)

#define spin_unlock(lock) do { (void) lock; } while (0)

#endif /* !NOSMP */

#endif /* !KERNEL_SPINLOCK_H_ */
