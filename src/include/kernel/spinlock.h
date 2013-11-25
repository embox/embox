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

#include <hal/ipl.h>
#include <linux/compiler.h>
#include <module/embox/arch/libarch.h>
#include <sys/types.h>
#include <kernel/sched/sched_lock.h>

#define SPIN_UNLOCKED 0
#define SPIN_LOCKED   1

#ifdef SMP

/**
 * spin_trylock -- try to lock object without waiting
 * @param lock  object to lock
 * @retval      1 if successfully blocked otherwise 1
 */
static inline int spin_trylock(spinlock_t *lock) {
	int ret;
	sched_lock();
#ifdef __HAVE_ARCH_CMPXCHG
	ret = (SPIN_UNLOCKED == cmpxchg(lock, SPIN_UNLOCKED, SPIN_LOCKED));
#else /* !__HAVE_ARCH_CMPXCHG */
	ret = __sync_bool_compare_and_swap(lock, SPIN_UNLOCKED, SPIN_LOCKED);
#endif /* __HAVE_ARCH_CMPXCHG */
	if (!ret)
		sched_unlock();
	return ret;
}

#else /* !SMP */

static inline int spin_trylock(spinlock_t *lock) {
	sched_lock();
	return SPIN_LOCKED;
}

#endif /* SMP */

/**
 * spin_lock -- try to lock object or wait until it's will done
 * @param lock  object to lock
 */
static inline void spin_lock(spinlock_t *lock) {
	while (!spin_trylock(lock))
		;
}

/**
 * spin_unlock -- unlock blocked object
 * @param lock  object to unlock
 */
static inline void spin_unlock(spinlock_t *lock) {
#ifdef SMP
	*lock = SPIN_UNLOCKED;
	__barrier();
#endif
	sched_unlock();
}

static inline ipl_t spin_lock_ipl(spinlock_t *lock) {
	ipl_t ipl = 0;

	while (1) {
		ipl = ipl_save();
		if (spin_trylock(lock))
			break;
		ipl_restore(ipl);
	}

	return ipl;
}

static inline void spin_unlock_ipl(spinlock_t *lock, ipl_t ipl) {
#ifdef SMP
	*lock = SPIN_UNLOCKED;
#endif /* SMP */
	ipl_restore(ipl);  /* implies optimization barrier */
	sched_unlock();
}


#endif /* !KERNEL_SPINLOCK_H_ */
