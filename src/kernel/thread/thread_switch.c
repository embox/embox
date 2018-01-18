/**
 * @file
 * @brief
 *
 * @date 6.08.2014
 * @author Vita Loginova
 */

#include <kernel/thread.h>
#include <kernel/addr_space.h>

static struct thread *saved_prev __cpudata__; // XXX

static struct context *prev_ctx __cpudata__;
static struct context *next_ctx __cpudata__;

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
	sched_timing_start(&thread_self()->schedee);
	sched_unlock();
}

static void thread_prepare_switch(struct thread *prev, struct thread *next) {
	sched_ticker_switch(prev->policy, next->policy);
	prev->critical_count = critical_count();
	critical_count() = next->critical_count;
	sched_start_switch(&next->schedee);
}

extern void thread_set_current(struct thread *t);
void thread_context_switch(struct thread *prev, struct thread *next) {
	thread_prepare_switch(prev, next);

	/* Preserve initial semantics of prev/next. */
	cpudata_var(saved_prev) = prev;

	thread_set_current(next);

	/* Store context pointers in static variables as after address space switch
	 * we may have corrupt stack before changing the stack pointer */
	cpudata_var(prev_ctx) = &prev->context;
	cpudata_var(next_ctx) = &next->context;

	ADDR_SPACE_PREPARE_SWITCH();

	/* implies cc barrier */
	context_switch(cpudata_var(prev_ctx), cpudata_var(next_ctx));

	ADDR_SPACE_FINISH_SWITCH();

	prev = cpudata_var(saved_prev);

	sched_finish_switch(&prev->schedee);
}

