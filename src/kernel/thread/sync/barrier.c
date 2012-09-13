/**
 * @file
 * @brief Implements barrier methods.
 *
 * @date 03.09.2012
 * @author Anton Bulychev
 */

#include <assert.h>
#include <errno.h>
#include <kernel/thread/sync/barrier.h>
#include <kernel/thread/sched.h>

void barrier_init(barrier_t *b, int count) {
	sleepq_init(&b->sq);
	b->current_count = 0;
	b->count = count;
}

void barrier_wait(barrier_t *b) {
	assert(b);
	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		if (b->count == b->current_count + 1) {
			b->current_count = 0;
			sched_wake_all(&b->sq);
		} else {
			b->current_count++;
			sched_sleep_locked(&b->sq, SCHED_TIMEOUT_INFINITE);
		}
	}
	sched_unlock();
}

