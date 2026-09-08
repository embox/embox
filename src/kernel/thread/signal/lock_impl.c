/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.01.2014
 */

#include <assert.h>
#include <hal/cpu.h>
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <kernel/critical.h>

#include <kernel/thread/signal_lock.h>

void *__threadsig_last_lock;
void *__threadsig_last_unlock;

void threadsig_lock(void) {
	struct thread *t = thread_self();

	assert(t);
	assert(t->siglock >= 0);

	/* Who took it last, so the assert below can name the pair rather than only
	 * the half that failed. Not per-thread on purpose: the question is which
	 * two call sites disagree, and a global pair answers it with one word of
	 * memory. */
	__threadsig_last_lock = __builtin_return_address(0);

	++t->siglock;

	assert(t->siglock > 0); /* overflow */
}

void threadsig_unlock(void) {
	struct thread *t = thread_self();
	struct schedee *s = schedee_get_current();

	assert(t);
	/* Who, where, and whether the running schedee is even this thread: on one
	 * core an unbalanced unlock is a bug in the caller, on four it is just as
	 * likely to be a thread that was not the one holding the lock. */
	assertf(t->siglock > 0,
	    "siglock %u th %p id %d cpu %u lk %p unlk %p%s", t->siglock, t, t->id,
	    cpu_get_id(), __threadsig_last_lock, __threadsig_last_unlock,
	    (s == &t->schedee) ? "" : " NOT-SELF");

	__threadsig_last_unlock = __builtin_return_address(0);

	--t->siglock;

	if (!t->siglock) {
		/* No signal could be handled in sched_lock and harder */
		if (critical_allows(CRITICAL_SCHED_LOCK)) {
			thread_signal_handle();
		} else {
			/* Ensure that signal will be handled after *_unlock()
 			 * of all locks (sched_lock and harder) */
			sched_post_switch();
		}
	}
}
