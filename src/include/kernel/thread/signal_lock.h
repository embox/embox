/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.12.2013
 */

#ifndef THREAD_SIGNAL_LOCK_H_
#define THREAD_SIGNAL_LOCK_H_

#include <assert.h>
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <kernel/critical.h>

static inline void threadsig_lock(void) {
	struct thread *t = thread_self();

	assert(t);
	assert(t->siglock >= 0);

	++t->siglock;

	assert(t->siglock > 0); /* overflow */
}

static inline void threadsig_unlock(void) {
	struct thread *t = thread_self();

	assert(t);
	assert(t->siglock > 0);

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

#endif /* THREAD_SIGNAL_LOCK_H_ */
