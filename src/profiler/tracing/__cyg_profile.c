#include <stdio.h>
#include <profiler/tracing/trace.h>
#include <kernel/critical.h>
#include <hal/ipl.h>

static inline int enter_condition() {
	return !critical_inside(CRITICAL_IRQ_LOCK) && !critical_inside(CRITICAL_IRQ_HANDLER) &&
			!critical_inside(CRITICAL_SOFTIRQ_LOCK)  && !critical_inside(CRITICAL_SOFTIRQ_HANDLER) &&
			!critical_inside(CRITICAL_SCHED_LOCK);
}

void __cyg_profile_func_enter(void *func, void *caller) {
	ipl_t ci;
	if (cyg_profiling) {
		cyg_profiling = false;
		ci = ipl_save();
		if (enter_condition())
			trace_block_func_enter(func);
		ipl_restore(ci);
		cyg_profiling = true;
	}
}

void __cyg_profile_func_exit(void *func, void *caller) {
	ipl_t ci;
	if (cyg_profiling) {
		cyg_profiling = false;
		ci = ipl_save();
		if (enter_condition())
			trace_block_func_exit(func);
		ipl_restore(ci);
		cyg_profiling = true;
	}
}

