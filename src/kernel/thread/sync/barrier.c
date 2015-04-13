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
#include <kernel/sched.h>
#include <kernel/thread/waitq.h>

void barrier_init(barrier_t *b, int count) {
	waitq_init(&b->wq);
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
			waitq_wakeup_all(&b->wq);
		} else {
			b->current_count++;

			WAITQ_WAIT_ONCE(&b->wq, SCHED_TIMEOUT_INFINITE);
		}
	}
	sched_unlock();
}
