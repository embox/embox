/**
 * @file
 * @brief
 *
 * @date 08.02.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <linux/compiler.h>
#include <sys/types.h>

#include <framework/mod/options.h>
#include <hal/cpu.h>
#include <hal/ipl.h>
#include <kernel/critical.h>

#include "spinlock.h"

#define SPIN_DEBUG OPTION_GET(BOOLEAN, spin_debug)

static_assert(SPIN_CONTENTION_LIMIT >= 0, "");

#if defined(SMP) || SPIN_DEBUG

#include <util/atomic_rmw.h>

#include <module/embox/arch/libarch.h> /* for __HAVE_ARCH_CMPXCHG */

int __spin_trylock(spinlock_t *lock) {
	int ret;
	unsigned int cpu_id = cpu_get_id();

	assertf(lock->owner != cpu_id, "Recursive lock of a spin owned by this "
	                               "CPU");

#ifdef __HAVE_ARCH_CMPXCHG
	ret = (__SPIN_UNLOCKED == cmpxchg(&lock->l, __SPIN_UNLOCKED, __SPIN_LOCKED));
#else  /* !__HAVE_ARCH_CMPXCHG */
	/* Acquire on success, so nothing inside the critical section can be
	 * observed before the lock is held; relaxed on failure, because a failed
	 * try orders nothing. The __sync_ builtin this replaces was a full seq_cst
	 * barrier -- correct, just stronger than an acquire has to be. */
	ret = atomic_rmw_try_lock(&lock->l, __SPIN_UNLOCKED, __SPIN_LOCKED);
#endif /* __HAVE_ARCH_CMPXCHG */

	if (ret) {
		assert(lock->owner == -1u);
		lock->owner = cpu_id;
#if SPIN_CONTENTION_LIMIT
		lock->contention_count = SPIN_CONTENTION_LIMIT;
#endif
	}
	else {
#if SPIN_CONTENTION_LIMIT
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
#endif
	}

	return ret;
}

void __spin_lock(spinlock_t *lock) {
	while (!__spin_trylock(lock)) {}
}

void __spin_unlock(spinlock_t *lock) {
	assertf(lock->l == __SPIN_LOCKED, "Unlocking a not locked spin");
	assertf(lock->owner == cpu_get_id(), "Unlocking a spin owned by another "
	                                     "CPU");
	lock->owner = -1u;
	/* A release store, which is what the XXX this replaces asked for:
	 * everything the critical section wrote must be visible to the next CPU
	 * that takes the lock before it sees the lock free. */
	atomic_rmw_store(&lock->l, __SPIN_UNLOCKED, __ATOMIC_RELEASE);
}

/* A spin region wants preemption off, not the BKL. Borrowing
 * CRITICAL_SCHED_LOCK's bits for it claimed the BKL by raising the count that
 * stands for it, so every interrupt landing in a spin region ran outside the
 * BKL believing it was inside. The preempt block is outside
 * __CRITICAL_BKL_MASK and still harder than CRITICAL_SCHED_LOCK, so it defers
 * sched_preempt() exactly as before. */
void __spin_preempt_disable(void) {
	/* A read-modify-write of a per-CPU count from a context that is still
	 * preemptible -- that is the whole point, it is about to stop being one.
	 * __spin_preempt_enable() needs no mask: by then the count is non-zero,
	 * which is itself the guarantee that nothing can migrate. */
	ipl_t ipl = ipl_save();

	__critical_count_add(__CRITICAL_COUNT(CRITICAL_PREEMPT_LOCK));
	ipl_restore(ipl);
}

void __spin_preempt_enable(void) {
	__critical_count_sub(__CRITICAL_COUNT(CRITICAL_PREEMPT_LOCK));
	critical_dispatch_pending();
}

#else /* !(SMP || SPIN_DEBUG) */

int __spin_trylock(spinlock_t *lock) {
	int ret;

	ret = (lock->owner != 0);

	if (ret) {
		lock->owner = 0;
#if SPIN_CONTENTION_LIMIT
		lock->contention_count = SPIN_CONTENTION_LIMIT;
#endif
	}
	else {
#if SPIN_CONTENTION_LIMIT
		lock->contention_count--;
		assertf(lock->contention_count > 0, "Possible spin deadlock");
#endif
	}

	return ret;
}

void __spin_lock(spinlock_t *lock) {
	while (!__spin_trylock(lock)) {}
}

void __spin_unlock(spinlock_t *lock) {
	lock->owner = -1u;
	__barrier();
}

void __spin_preempt_disable(void) {
	__critical_count_add(__CRITICAL_COUNT(CRITICAL_SCHED_LOCK));
}

void __spin_preempt_enable(void) {
	__critical_count_sub(__CRITICAL_COUNT(CRITICAL_SCHED_LOCK));
	critical_dispatch_pending();
}

#endif /* SMP || SPIN_DEBUG */

void spin_init(spinlock_t *lock, unsigned int state) {
	lock->owner = -1u;
	lock->l = state;
#if SPIN_CONTENTION_LIMIT
	lock->contention_count = SPIN_CONTENTION_LIMIT;
#endif
}

/**
 * spin_trylock -- try to lock object without waiting
 * @param lock  object to lock
 * @retval      1 if successfully blocked otherwise 1
 */
int spin_trylock(spinlock_t *lock) {
	int ret;
	__spin_preempt_disable();
	ret = __spin_trylock(lock);
	if (!ret) {
		__spin_preempt_enable();
	}
	return ret;
}

/**
 * spin_lock -- try to lock object or wait until it's will done
 * @param lock  object to lock
 */
void spin_lock(spinlock_t *lock) {
	while (!spin_trylock(lock)) {}
}

/**
 * spin_unlock -- unlock blocked object
 * @param lock  object to unlock
 */
void spin_unlock(spinlock_t *lock) {
	__spin_unlock(lock);
	__spin_preempt_enable();
}

ipl_t spin_lock_ipl(spinlock_t *lock) {
	ipl_t ipl = 0;

	while (1) {
		ipl = ipl_save();
		if (spin_trylock(lock)) {
			break;
		}
		ipl_restore(ipl);
	}

	return ipl;
}

void spin_unlock_ipl(spinlock_t *lock, ipl_t ipl) {
	__spin_unlock(lock);
	ipl_restore(ipl); /* implies optimization barrier */
	__spin_preempt_enable();
}

void spin_lock_ipl_disable(spinlock_t *lock) {
	ipl_t ipl = 0;

	while (1) {
		ipl = ipl_save();
		if (spin_trylock(lock)) {
			break;
		}
		ipl_restore(ipl);
	}
}

void spin_unlock_ipl_enable(spinlock_t *lock) {
	__spin_unlock(lock);
	ipl_enable(); /* implies optimization barrier */
	__spin_preempt_enable();
}
