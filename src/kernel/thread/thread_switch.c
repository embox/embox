/**
 * @file
 * @brief
 *
 * @date 6.08.2014
 * @author Vita Loginova
 */

#include <kernel/thread.h>
#include <kernel/addr_space.h>
#include <profiler/tracing/trace.h>

static struct thread *saved_prev __cpudata__; // XXX
static struct thread *saved_next __cpudata__; // XXX

/**
 * Any fresh thread must call this function from a trampoline.
 * Basically it emulates returning from the scheduler as it would be done
 * in case if 'context_switch' would return as usual (into 'sched_switch',
 * from where it was called) instead of jumping into a thread trampoline.
 */

void thread_ack_switched(void) {
	ADDR_SPACE_FINISH_SWITCH();
	sched_finish_switch(&cpudata_var(saved_prev)->schedee);
	ipl_enable();
	sched_unlock();
}

static void thread_prepare_switch(struct thread *prev, struct thread *next) {
	sched_ticker_switch(&prev->schedee, &next->schedee);
	prev->critical_count = critical_count();
	critical_count() = next->critical_count;
	sched_start_switch(&next->schedee);
}

struct thread *thread_switch(struct thread *prev, struct thread *next) {
	thread_prepare_switch(prev, next);

	trace_point(__func__);

	/* Preserve initial semantics of prev/next. */
	cpudata_var(saved_prev) = prev;
	cpudata_var(saved_next) = next;
	ADDR_SPACE_PREPARE_SWITCH();

	context_switch(&prev->context, &next->context);  /* implies cc barrier */

	ADDR_SPACE_FINISH_SWITCH();
	prev = cpudata_var(saved_prev);
	next = cpudata_var(saved_next);

	sched_finish_switch(&prev->schedee);

	return next;
}

