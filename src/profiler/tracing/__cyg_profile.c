#include <stdio.h>
#include <profiler/tracing/trace.h>

void __cyg_profile_func_enter(void *func, void *caller) {
	if (cyg_profiling && !critical_inside()) {
		cyg_profiling = false;
		trace_block_func_enter(func);
		cyg_profiling = true;
	}
}

void __cyg_profile_func_exit(void *func, void *caller) {
	if (cyg_profiling) {
		cyg_profiling = false;
		trace_block_func_exit(func);
		cyg_profiling = true;
	}
}

