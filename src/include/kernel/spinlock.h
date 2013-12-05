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

#include <sys/types.h>
#include <linux/compiler.h>

#include <hal/cpu.h>
#include <hal/ipl.h>
#include <kernel/critical.h>
#include <module/embox/arch/libarch.h>

#include <util/lang.h>
#include <util/macro.h>

#define SPIN_UNLOCKED 0
#define SPIN_LOCKED   1

#ifdef SMP

static inline int spin_trylock_no_preempt(spinlock_t *lock) {
#ifdef __HAVE_ARCH_CMPXCHG
	return (SPIN_UNLOCKED == cmpxchg(lock, SPIN_UNLOCKED, SPIN_LOCKED));
#else /* !__HAVE_ARCH_CMPXCHG */
	return __sync_bool_compare_and_swap(lock, SPIN_UNLOCKED, SPIN_LOCKED);
#endif /* __HAVE_ARCH_CMPXCHG */
}

#else /* !SMP */

static inline int spin_trylock_no_preempt(spinlock_t *lock) {
	return SPIN_LOCKED;
}

#endif /* SMP */

static inline void spin_lock_no_preempt(spinlock_t *lock) {
	while (!spin_trylock_no_preempt(lock))
		;
}

static inline void spin_unlock_no_preempt(spinlock_t *lock) {
#ifdef SMP
	*lock = SPIN_UNLOCKED;
#endif
	__barrier();
}

/**
 * spin_trylock -- try to lock object without waiting
 * @param lock  object to lock
 * @retval      1 if successfully blocked otherwise 1
 */
static inline int spin_trylock(spinlock_t *lock) {
	int ret;
	__critical_count_add(__CRITICAL_COUNT(CRITICAL_SCHED_LOCK));
	ret = spin_trylock_no_preempt(lock);
	if (!ret)
		__critical_count_sub(__CRITICAL_COUNT(CRITICAL_SCHED_LOCK));
	return ret;
}

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
	__critical_count_sub(__CRITICAL_COUNT(CRITICAL_SCHED_LOCK));
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
	__critical_count_sub(__CRITICAL_COUNT(CRITICAL_SCHED_LOCK));
}

static inline void spin_lock_ipl_disable(spinlock_t *lock) {
	ipl_t ipl = 0;

	while (1) {
		ipl = ipl_save();
		if (spin_trylock(lock))
			break;
		ipl_restore(ipl);
	}
}

static inline void spin_unlock_ipl_enable(spinlock_t *lock) {
#ifdef SMP
	*lock = SPIN_UNLOCKED;
#endif /* SMP */
	ipl_enable();  /* implies optimization barrier */
	__critical_count_sub(__CRITICAL_COUNT(CRITICAL_SCHED_LOCK));
}

/**
 * Spin until either @a lock is acquired or @a cond becomes @c false.
 * @return @a cond value. In case of a non-zero value the spin is locked.
 * Unlocking it in the latter case is up to the client.
 */
#define SPIN_LOCK_COND(lock, cond) \
	({                                       \
		spinlock_t *__lock = (lock);         \
		typeof(cond) __cond;                 \
		                                     \
		do {                                 \
			__cond = (cond);                 \
			if (!__cond)                     \
				break;                       \
		} while (!spin_trylock(__lock));     \
		                                     \
		if (__cond) {                        \
			/* just been locked */           \
			__cond = (cond);                 \
			if (!__cond)                     \
				spin_unlock(__lock);         \
		}                                    \
		                                     \
		__cond;                              \
	})

/**
 * 'if' statement based on #SPIN_LOCK_COND(). True branch gets executed with
 * a non-zero value of @a cond and spin locked. False branch gets no locks.
 * No explicit unlocking is required.
 * Making 'return' stmt inside the block will leave the spin locked.
 * Also 'break'/'continue' will not work as expected.
 */
#define spin_protected_if(lock, cond) \
	__spin_protected_if(lock, cond, \
		MACRO_GUARD(__done), \
		MACRO_GUARD(__lock), \
		MACRO_GUARD(__cond))

#define __spin_protected_if(lock, cond, __done, __lock, __cond) \
	for (int __done = 0;                              !__done; ) \
	for (spinlock_t *__lock = (lock);                 !__done; ) \
	for (int __cond = !!SPIN_LOCK_COND(__lock, cond); !__done;   \
			({ if (__cond) spin_unlock(__lock); }))              \
	while (!__done && (__done = 1)) /* break/continue control this loop */  \
		if (__cond)

#define SPIN_PROTECTED_DO(lock, expr) \
	__lang_surround(expr,             \
		spinlock_t *__lock = (lock);  \
		spin_lock(__lock),            \
		spin_unlock(__lock))

#define SPIN_IPL_PROTECTED_DO(lock, expr) \
	__lang_surround(expr,                    \
		spinlock_t *__lock = (lock);         \
		ipl_t __ipl = spin_lock_ipl(__lock), \
		spin_unlock_ipl(__lock, __ipl))

#endif /* !KERNEL_SPINLOCK_H_ */
