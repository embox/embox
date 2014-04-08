#include <stdio.h>
#include <profiler/tracing/trace.h>

static inline int enter_condition() {
	return 1;
	/*return !critical_inside(CRITICAL_IRQ_LOCK) && !critical_inside(CRITICAL_IRQ_HANDLER) &&
			!critical_inside(CRITICAL_SOFTIRQ_LOCK)  && !critical_inside(CRITICAL_SOFTIRQ_HANDLER) &&
			!critical_inside(CRITICAL_SCHED_LOCK);
*/}

void __cyg_profile_func_enter(void *func, void *caller) {
	if (cyg_profiling) {
		cyg_profiling = false;
		if (enter_condition())
			trace_block_func_enter(func);
		cyg_profiling = true;
	}
}

void __cyg_profile_func_exit(void *func, void *caller) {
	if (cyg_profiling) {
		cyg_profiling = false;
		if (enter_condition())
			trace_block_func_exit(func);
		cyg_profiling = true;
	}
}

