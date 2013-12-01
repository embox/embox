/*
	Thread planning functions, used in sched_switch

	Implements runnable.h interface
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

void sched_prepare_runnable(struct runnable *p, struct runq *rq) {
	struct thread *prev;

	prev = (struct thread *)p;

	if (is_running(prev) && !is_waiting(prev)) {
		make_ready(prev);
		runq_insert(&(rq->queue), p);
	}
}

void sched_execute_runnable(struct runnable *p, struct runnable *n, struct runq *rq) {
	struct thread *prev, *next;

	prev = (struct thread *)p;
	next = (struct thread *)n;

	prev->state &= ~__THREAD_STATE_RUNNING;

	assert(next != NULL);
	assert(is_ready(next));

	make_running(next);

	if (prev == next) {
		ipl_disable();
		return;
	}

	sched_ticker_switch(prev, next);
	sched_timing_switch(prev, next);

	trace_point("context switch");

	thread_set_current(next);
	context_switch(&prev->context, &next->context);
}

struct runnable *runnable_get_current(){
	return &(thread_get_current()->runnable);
}
