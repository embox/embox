/*
	Thread planning functions, used in sched_switch

	Implements runnable interface
*/

#include <hal/context.h>
#include <hal/ipl.h>
#include <stdlib.h>
#include <assert.h>
#include <sched.h>
#include <kernel/sched.h>
#include <kernel/critical.h>
#include <kernel/thread.h>
#include <kernel/runnable/runnable.h>
#include <kernel/thread/state.h>
#include <profiler/tracing/trace.h>

void sched_thread_specific(struct runnable *p, struct runq *rq) {
	struct thread *prev;

	prev = (struct thread *)p;

	if (prev->state & __THREAD_STATE_RUNNING) {
			runq_queue_insert(&rq->queue, p);
	}
}

void sched_execute_thread(struct runnable *p, struct runnable *n, struct runq *rq) {
	struct thread *prev, *next;
	clock_t new_clock;

	prev = (struct thread *)p;
	next = (struct thread *)n;

	if (prev == next) {
		ipl_disable();
		return;
	} else {
		if (prev->state & __THREAD_STATE_RUNNING) {
			prev->state |= __THREAD_STATE_READY;
			/* TODO maybe without waiting */
			prev->state &= ~(__THREAD_STATE_RUNNING | __THREAD_STATE_WAITING);
		}
		next->state |= __THREAD_STATE_RUNNING;
		/* TODO maybe without waiting */
		next->state &= ~(__THREAD_STATE_READY | __THREAD_STATE_WAITING);
	}

	if (prev->runnable.policy == SCHED_FIFO && next->runnable.policy != SCHED_FIFO) {
		sched_ticker_init();
	}

	if (prev->runnable.policy != SCHED_FIFO && next->runnable.policy == SCHED_FIFO) {
		sched_ticker_fini(rq);
	}

	/* Running time recalculation */
	new_clock = clock();
	sched_timing_stop(prev, new_clock);
	sched_timing_start(next, new_clock);

	trace_point("context switch");

	ipl_disable();

	thread_set_current(next);
	context_switch(&(prev->context), &(next->context));
}

