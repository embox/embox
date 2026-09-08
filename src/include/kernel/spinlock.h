/**
 * @file
 * @brief
 *
 * @date 08.02.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_SPINLOCK_H_
#define KERNEL_SPINLOCK_H_

#include <assert.h>

#include <sys/types.h>
#include <linux/compiler.h>

#include <hal/cpu.h>
#include <hal/ipl.h>
#include <kernel/critical.h>
#include <module/embox/arch/libarch.h>

#include <util/lang.h>
#include <util/macro.h>

#include <framework/mod/options.h>
#include <config/embox/kernel/spinlock.h>
#include <util/atomic_rmw.h>
#if OPTION_MODULE_GET(embox__kernel__spinlock, BOOLEAN, spin_debug)
#define SPIN_DEBUG
#endif
#define SPIN_CONTENTION_LIMIT 0x10000000

#ifdef SPIN_CONTENTION_LIMIT
# define __SPIN_CONTENTION_FIELD      unsigned long contention_count;
# define __SPIN_CONTENTION_FIELD_INIT contention_count : SPIN_CONTENTION_LIMIT,
#else /* SPIN_CONTENTION_LIMIT */
# define __SPIN_CONTENTION_FIELD
# define __SPIN_CONTENTION_FIELD_INIT
#endif /* SPIN_CONTENTION_LIMIT */

typedef struct {
	unsigned long l;
	unsigned int owner;
	__SPIN_CONTENTION_FIELD
} spinlock_t;

/* XXX use 'field : value' instead of '.field = value' syntax because g++ does not support
 * the second one, but supports the first one in the trivial order --Alexander */
#define SPIN_INIT(state) \
  { l : state, owner : -1u, __SPIN_CONTENTION_FIELD_INIT }

static inline void spin_init(spinlock_t *lock, unsigned int state) {
	lock->l = state;
#ifdef SPIN_CONTENTION_LIMIT
	lock->contention_count = SPIN_CONTENTION_LIMIT;
#endif
	lock->owner = -1u;
}

#define SPIN_STATIC_UNLOCKED SPIN_INIT(__SPIN_UNLOCKED)
#define SPIN_STATIC_LOCKED   SPIN_INIT(__SPIN_LOCKED)

#define SPIN_UNLOCKED (spinlock_t) SPIN_STATIC_UNLOCKED
#define SPIN_LOCKED   (spinlock_t) SPIN_STATIC_LOCKED

#define __SPIN_UNLOCKED 0
#define __SPIN_LOCKED   1

#if defined(SMP) || defined(SPIN_DEBUG)

static inline int __spin_trylock_smp(spinlock_t *lock) {
#ifdef __HAVE_ARCH_CMPXCHG
	return (__SPIN_UNLOCKED == cmpxchg(&lock->l, __SPIN_UNLOCKED, __SPIN_LOCKED));
#else /* !__HAVE_ARCH_CMPXCHG */
	/* Acquire on success, so nothing inside the critical section can be
	 * observed before the lock is held; relaxed on failure, because a failed
	 * try orders nothing. The __sync_ builtin this replaces was a full seq_cst
	 * barrier -- correct, just stronger than an acquire has to be. */
	return atomic_rmw_try_lock(&lock->l, __SPIN_UNLOCKED, __SPIN_LOCKED);
#endif /* __HAVE_ARCH_CMPXCHG */
}

#else /* !(SMP || SPIN_DEBUG) */

static inline int __spin_trylock_smp(spinlock_t *lock) {
	(void)lock;

	return 1;
}

#endif /* SMP || SPIN_DEBUG */

static inline int __spin_trylock(spinlock_t *lock) {
	int ret;

#if defined(SMP) || defined(SPIN_DEBUG)
	unsigned int cpu_id = cpu_get_id();

	assertf(lock->owner != cpu_id, "Recursive lock of a spin owned by this CPU");
#endif

	ret = __spin_trylock_smp(lock);
#if defined(SMP) || defined(SPIN_DEBUG)
	if (ret) {
		assert(lock->owner == -1u);
		lock->owner = cpu_id;
	}
#endif
#ifdef SPIN_CONTENTION_LIMIT
	if (ret)
		lock->contention_count = SPIN_CONTENTION_LIMIT;
	else {
		/* Every CPU spinning on this lock decrements the same field; a lost
		 * update makes the counter reach zero early and the detector reports
		 * a deadlock that is not there. Relaxed: this counts, it orders
		 * nothing. */
		unsigned long left;

		left = atomic_rmw_sub_fetch(&lock->contention_count, 1, __ATOMIC_RELAXED);
		/* Name the lock and its owner: "waited too long" on its own is not a
		 * diagnosis, which lock and who holds it are. */
		assertf(left, "Possible spin deadlock: lock %p held by cpu %u", lock,
		    lock->owner);
	}
#endif
	return ret;
}

static inline void __spin_lock(spinlock_t *lock) {
	while (!__spin_trylock(lock))
		;
}

static inline void __spin_unlock(spinlock_t *lock) {
#if defined(SMP) || defined(SPIN_DEBUG)
	assertf(lock->l == __SPIN_LOCKED, "Unlocking a not locked spin");
	assertf(lock->owner == cpu_get_id(), "Unlocking a spin owned by another CPU");
	lock->owner = -1u;
	/* A release store, which is what the XXX this replaces asked for:
	 * everything the critical section wrote must be visible to the next CPU
	 * that takes the lock before it sees the lock free. */
	atomic_rmw_store(&lock->l, __SPIN_UNLOCKED, __ATOMIC_RELEASE);
#else /* !(SMP || SPIN_DEBUG) */
	__barrier();
#endif /* SMP || SPIN_DEBUG */
}

/* A spin region wants preemption off, not the BKL. Borrowing
 * CRITICAL_SCHED_LOCK's bits for it claimed the BKL by raising the count that
 * stands for it, so every interrupt landing in a spin region ran outside the
 * BKL believing it was inside. The preempt block is outside
 * __CRITICAL_BKL_MASK and still harder than CRITICAL_SCHED_LOCK, so it defers
 * sched_preempt() exactly as before. */
static inline void __spin_preempt_disable(void) {
	/* A read-modify-write of a per-CPU count from a context that is still
	 * preemptible -- that is the whole point, it is about to stop being one.
	 * __spin_preempt_enable() needs no mask: by then the count is non-zero,
	 * which is itself the guarantee that nothing can migrate. */
	ipl_t ipl = ipl_save();

	__critical_count_add(__CRITICAL_COUNT(CRITICAL_PREEMPT_LOCK));
	ipl_restore(ipl);
}

static inline void __spin_preempt_enable(void) {
	__critical_count_sub(__CRITICAL_COUNT(CRITICAL_PREEMPT_LOCK));
	critical_dispatch_pending();
}

/**
 * spin_trylock -- try to lock object without waiting
 * @param lock  object to lock
 * @retval      1 if successfully blocked otherwise 1
 */
static inline int spin_trylock(spinlock_t *lock) {
	int ret;
	__spin_preempt_disable();
	ret = __spin_trylock(lock);
	if (!ret)
		__spin_preempt_enable();
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
	__spin_unlock(lock);
	__spin_preempt_enable();
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
	__spin_unlock(lock);
	ipl_restore(ipl);  /* implies optimization barrier */
	__spin_preempt_enable();
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
	__spin_unlock(lock);
	ipl_enable();  /* implies optimization barrier */
	__spin_preempt_enable();
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
	while (!__done && (++__done)) /* break/continue control this loop */  \
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
