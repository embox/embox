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
			struct waitq_link wql;

			b->current_count++;

			waitq_link_init(&wql);

			waitq_wait_prepare(&b->wq, &wql);
			sched_wait();
			waitq_wait_cleanup(&b->wq, &wql);
		}
	}
	sched_unlock();
}
