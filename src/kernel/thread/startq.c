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

void startq_dequeue_thread(struct startq_data *startq_data);
void startq_dequeue_sleepq(struct startq_data *startq_data);

static DLIST_DEFINE(startq);

void startq_init_thread(struct startq_data *startq_data) {
	dlist_head_init(&startq_data->startq_link);
	startq_data->in_list = 0;
	startq_data->dequeue = startq_dequeue_thread;
}

void startq_init_sleepq(struct startq_data *startq_data) {
	dlist_head_init(&startq_data->startq_link);
	startq_data->in_list = 0;
	startq_data->dequeue = startq_dequeue_sleepq;
}

/**
 * Called outside any interrupt handler as part of critical dispatcher
 * with IRQs disabled and the scheduler locked.
 */
void startq_flush(void) {
	struct startq_data *startq_data;

	assert(!critical_allows(CRITICAL_SCHED_LOCK));
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	/* Run through all items in startq. */
	while (!dlist_empty(&startq)) {
		/* Get startq data of first item. */
		startq_data = dlist_entry(startq.next, struct startq_data, startq_link);
		startq_data->dequeue(startq_data);
	}
}

void startq_enqueue_sleepq(struct sleepq *sleepq, int wake_all) {
	struct startq_data *startq_data = &sleepq->startq_data;
	ipl_t ipl = ipl_save();
	{
		/* Add if sleepq is not in startq list. */
		if (!startq_data->in_list) {
			/* Add sleepq to the end of the list. */
			dlist_add_prev(&startq_data->startq_link, &startq);
			startq_data->in_list = 1;

			/* Set up wake all. */
			startq_data->data = wake_all;
		} else {
			/* Already in list. Checks if we should wake all sleepq. */
			if (wake_all) {
				startq_data->data = 1;
			}
		}
	}
	ipl_restore(ipl);
}

void startq_enqueue_thread(struct thread *thread, int sleep_result) {
	struct startq_data *startq_data = &thread->startq_data;
	ipl_t ipl = ipl_save();
	{
		/* Add if thread is not in startq list. */
		if (!startq_data->in_list) {
			/* Add thread to the end of the list. */
			dlist_add_prev(&startq_data->startq_link, &startq);
			startq_data->in_list = 1;

			/* Remember sleep result. It will be set in sleepq_flush(). */
			startq_data->data = sleep_result;
		}
	}
	ipl_restore(ipl);
}

void startq_dequeue_thread(struct startq_data *startq_data) {
	int sleep_res;
	/* Get pointer to the thread. */
	struct thread *thread = dlist_entry(&startq_data->startq_link,
			struct thread, startq_data.startq_link);

	/* Delete thread from the startq list. */
	dlist_del(&startq_data->startq_link);
	startq_data->in_list = 0;

	/* Check if thread sleeping. It was added inside interruption and
	 * we could not check this at that time. */
	if (thread_state_sleeping(thread->state)) {
		/* Remember sleep result before enabling interruption. */
		sleep_res = startq_data->data;

		/* Enable interrupts, wake up thread and disable again. */
		ipl_enable();
		do_wake_thread(thread, sleep_res);
		ipl_disable();
	}
}

void startq_dequeue_sleepq(struct startq_data *startq_data) {
	int wake_all;
	/* Get pointer to the sleepq. */
	struct sleepq *sleepq = dlist_entry(&startq_data->startq_link,
			struct sleepq, startq_data.startq_link);

	/* Delete sleepq from the startq list. */
	dlist_del(&startq_data->startq_link);
	startq_data->in_list = 0;

	/* Remember wake_all before enabling interruption. */
	wake_all = startq_data->data;

	/* Enable interrupts, wake up sleepq and disable again. */
	ipl_enable();
	do_wake_sleepq(sleepq, wake_all);
	ipl_disable();
}
