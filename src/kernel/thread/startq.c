/**
 * @file
 * @brief TODO
 *
 * @date 13.09.2012
 * @author Anton Bulychev
 */

#include <hal/ipl.h>
#include <kernel/critical.h>
#include <kernel/thread/startq.h>
#include <kernel/thread/state.h>
#include <util/dlist.h>
#include "types.h"

extern void do_wake_thread(struct thread *thread, int sleep_result);
extern void do_wake_sleepq(struct sleepq *sleepq, int wake_all);

static DLIST_DEFINE(startq);

/**
 * Called outside any interrupt handler as part of critical dispatcher
 * with IRQs disabled and the scheduler locked.
 */
void startq_flush(void) {
	struct thread *t;
	struct sleepq *sq;
	struct startq_data *sd;
	int wake_all;
	int sleep_res;

	assert(!critical_allows(CRITICAL_SCHED_LOCK));
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	/* Run through all items in startq. */
	while (!dlist_empty(&startq)) {
		/* Get startq data of first item. */
		sd = dlist_entry(startq.next, struct startq_data, startq_link);

		/* Determine type of item. */
		if (sd->info == STARTQ_WAKE_THREAD) {
			/* Item is a thread, get it. */
			t = dlist_entry(startq.next, struct thread, startq_data.startq_link);

			/* Delete item from the startq list. */
			dlist_del(&sd->startq_link);
			sd->info = STARTQ_DATA_NOT_IN_LIST;

			/* Check if thread sleeping. It was added inside interruption and
			 * we could not check this at that time. */
			if (thread_state_sleeping(t->state)) {
				/* Remember sleep result before enabling interruption. */
				sleep_res = sd->sleep_res;

				/* Enable interrupts, wake up thread and disable again. */
				ipl_enable();
				do_wake_thread(t, sleep_res);
				ipl_disable();
			}
		} else {
			/* Item is a sleepq, get it. */
			sq = dlist_entry(startq.next, struct sleepq, startq_data.startq_link);

			/* Should we wake up all sleepq? */
			if (sd->info == STARTQ_WAKE_SLEEPQ_ALL) {
				wake_all = 1;
			} else {
				wake_all = 0;
			}

			/* Delete item from the startq list. */
			dlist_del(&sd->startq_link);
			sd->info = STARTQ_DATA_NOT_IN_LIST;

			/* Enable interrupts, wake up sleepq and disable again. */
			ipl_enable();
			do_wake_sleepq(sq, wake_all);
			ipl_disable();
		}
	}
}

void startq_enqueue_sleepq(struct sleepq *sq, int wake_all) {
	ipl_t ipl = ipl_save();
	{
		/* Add if sleepq is not in startq list. */
		if (sq->startq_data.info == STARTQ_DATA_NOT_IN_LIST) {
			/* Add sleepq to the end of the list. */
			dlist_add_prev(&sq->startq_data.startq_link, &startq);

			/* Set up startq data info. */
			if (wake_all) {
				sq->startq_data.info = STARTQ_WAKE_SLEEPQ_ALL;
			} else {
				sq->startq_data.info = STARTQ_WAKE_SLEEPQ_ONE;
			}
		} else {
			/* Already in list. Checks if we should wake all sleepq. */
			if (wake_all) {
				sq->startq_data.info = STARTQ_WAKE_SLEEPQ_ALL;
			}
		}
	}
	ipl_restore(ipl);
}

void startq_enqueue_thread(struct thread *t, int sleep_result) {
	ipl_t ipl = ipl_save();
	{
		/* Add if thread is not in startq list. */
		if (t->startq_data.info == STARTQ_DATA_NOT_IN_LIST) {
			/* Add thread to the end of the list. */
			dlist_add_prev(&t->startq_data.startq_link, &startq);

			/* Set up startq data info. */
			t->startq_data.info = STARTQ_WAKE_THREAD;

			/* Remember sleep result. It will be set in sleepq_flush(). */
			t->startq_data.sleep_res = sleep_result;
		}
	}
	ipl_restore(ipl);
}
