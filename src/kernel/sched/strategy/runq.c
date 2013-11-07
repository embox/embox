/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#include <framework/mod/options.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include <kernel/thread.h>
#include <kernel/sched.h>
#include <kernel/sched/sched_strategy.h>
#include <kernel/thread/state.h>

#include <kernel/task.h>

#include <kernel/sched/runq.h>


void sched_strategy_init(struct thread *t) {
	runq_item_init(&t->sched_attr.runq_link);
	sched_affinity_init(t);
	sched_timing_init(t);
}

struct thread *runq_switch(struct runq *rq) {
	struct thread *current = thread_self();
	struct thread *next;

	assert(rq);

	if (thread_state_running(current->state)) {
		runq_queue_insert(&rq->queue, current);
	}

	next = runq_queue_extract(&rq->queue);

	assert(next != NULL);
	assert(thread_state_running(next->state));

	if (next != current) {
		current->state = thread_state_do_outcpu(current->state);
		next->state = thread_state_do_oncpu(next->state);
	}

	return next;
}
