/**
 * @file
 * @brief
 *
 * @date 6.08.2014
 * @author Vita Loginova
 */

#include <util/log.h>

#include <hal/cpu.h>

#include <kernel/thread.h>
#include <kernel/addr_space.h>

static struct thread *saved_prev __cpudata__; // XXX

/**
 * Any fresh thread must call this function from a trampoline.
 * Basically it emulates returning from the scheduler as it would be done
 * in case if 'context_switch' would return as usual (into 'sched_switch',
 * from where it was called) instead of jumping into a thread trampoline.
 */

void thread_ack_switched(void) {
	ADDR_SPACE_FINISH_SWITCH();
	sched_finish_switch(&cpudata_var(saved_prev)->schedee);
	log_debug("first switch on cpu :%d",cpu_get_id());
	ipl_enable();
	/* we add first timer the same behavious as sched_ticker_update*/
	sched_ticker_add(cpu_get_id());
	sched_timing_start(&thread_self()->schedee);	
	sched_unlock();
}

static void thread_prepare_switch(struct thread *prev, struct thread *next) {
	prev->critical_count = critical_count();
	critical_count() = next->critical_count;
	sched_start_switch(&next->schedee);
	log_debug("thread prepare switch on cpu :%d",cpu_get_id());
}

extern void thread_set_current(struct thread *t);
/** Locks: IPL. */
void thread_context_switch(struct thread *prev, struct thread *next) {
	thread_prepare_switch(prev, next);

	/* Preserve initial semantics of prev/next. */
	cpudata_var(saved_prev) = prev;
	ADDR_SPACE_PREPARE_SWITCH();

	thread_set_current(next);
	context_switch(&prev->context, &next->context);  /* implies cc barrier */

	ADDR_SPACE_FINISH_SWITCH();
	prev = cpudata_var(saved_prev);

	sched_finish_switch(&prev->schedee);
	log_debug("thread finish switch on cpu :%d",cpu_get_id());
}

