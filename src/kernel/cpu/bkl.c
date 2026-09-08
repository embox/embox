/**
 * @file
 * @brief Implementation of Big Kernel Lock
 *
 * @date 08.02.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 */

#include <assert.h>

#include <hal/ipl.h>
#include <hal/cpu.h>
#include <kernel/spinlock.h>
#include <kernel/thread.h>
#include <util/atomic_rmw.h>

static spinlock_t bkl = SPIN_STATIC_UNLOCKED;

/* Which CPU holds the BKL. Kept here because spinlock_t's owner field is
 * only maintained under SMP/SPIN_DEBUG; on a uniprocessor build it stays -1u
 * forever, so an assertion on it could never pass. Written by the CPU that
 * takes the lock, cleared by the one that releases it. */
static unsigned int bkl_owner = -1u;

void bkl_lock(void) {
	__spin_lock(&bkl);
	bkl_owner = cpu_get_id();
}

void bkl_unlock(void) {
	/* Cleared before the release: after it the next holder may already have
	 * written its own id, and this store would undo it. */
	bkl_owner = -1u;
	__spin_unlock(&bkl);
}

int bkl_trylock(void) {
	if (!__spin_trylock(&bkl)) {
		return 0;
	}
	bkl_owner = cpu_get_id();

	return 1;
}

void bkl_wait(void) {
	int spins = 64;

	/* Read-spin: the line stays shared, so a waiting core does not fight the
	 * holder for it the way a compare-exchange loop does. Bounded, and with no
	 * accounting of its own, so the caller's trylock keeps feeding the
	 * contention detector. */
	while (spins-- > 0
	       && atomic_rmw_load(&bkl.l, __ATOMIC_RELAXED) != __SPIN_UNLOCKED) {
	}
}

int bkl_owned(void) {
	return bkl_owner == cpu_get_id();
}

/* A non-zero critical count on this CPU is a promise that this CPU holds the
 * BKL. Reaching a nested critical_enter()/critical_leave() without owning it
 * is that promise being broken. */
void bkl_assert_owned(unsigned int tested, unsigned int unit, void *from) {
	/* `from` is the return address, which names the call site that broke the
	 * invariant. It is passed in rather than taken here, because at -O0
	 * critical_enter() is a real function and its own return address would
	 * name only itself. `tested` is the count the branch looked at,
	 * `critical_count()` the count now; they differ only if this CPU changed
	 * hands in between. */
	assertf(bkl_owner == cpu_get_id(),
	    "c %#x/%#x u %#x cpu %u bkl %u th %p st %#x @%p", tested,
	    (unsigned int)critical_count(), unit, cpu_get_id(), bkl_owner,
	    thread_self(), thread_self() ? thread_self()->state : 0, from);
}
