/**
 * @file
 * @brief TODO
 *
 * @date 13.09.2012
 * @author Anton Bulychev
 */

#include <hal/ipl.h>
#include <kernel/critical.h>
#include <util/slist.h>
#include "types.h"

#define STARTQ_TYPE_SLEEPQ_ONE 0
#define STARTQ_TYPE_SLEEPQ_ALL 1
#define STARTQ_TYPE_THREAD     2

extern int do_thread_wake_force(struct thread *thread);
extern void do_event_wake(struct sleepq *sleepq, int wake_all);

struct startq {
	struct slist sleepq_wake;
	struct slist thread_force_wake;
};

static struct startq startq = {
	.sleepq_wake = SLIST_INIT(&startq.sleepq_wake),
	.thread_force_wake = SLIST_INIT(&startq.thread_force_wake),

};

/**
 * Called outside any interrupt handler as part of critical dispatcher
 * with IRQs disabled and the scheduler locked.
 */
void startq_flush(void) {
	struct thread *t;
	struct sleepq *sq;

	assert(!critical_allows(CRITICAL_SCHED_LOCK));
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	while ((sq = slist_remove_first(&startq.sleepq_wake,
			struct sleepq, startq_link))) {
		int wake_all = sq->startq_wake_all;

		ipl_enable();
		do_event_wake(sq, wake_all);
		ipl_disable();
	}

	while ((t = slist_remove_first(&startq.thread_force_wake,
			struct thread, startq_link))) {

		ipl_enable();
		do_thread_wake_force(t);
		ipl_disable();
	}
}

void startq_enqueue_wake(struct sleepq *sq, int wake_all) {
	ipl_t ipl = ipl_save();
	{
		if (slist_alone(sq, startq_link)) {
			slist_add_first(sq, &startq.sleepq_wake, startq_link);
			sq->startq_wake_all = wake_all;
		} else {
			sq->startq_wake_all |= wake_all;
		}
	}
	ipl_restore(ipl);
}

void startq_enqueue_wake_force(struct thread *t) {
	ipl_t ipl = ipl_save();
	{
		if (slist_alone(t, startq_link)) {
			slist_add_first(t, &startq.thread_force_wake, startq_link);
		}
	}
	ipl_restore(ipl);
}
