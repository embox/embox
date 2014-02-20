#include <stdio.h>

#include <profiler/tracing/trace.h>

void __cyg_profile_func_enter(void *func, void *caller) {
	trace_block_func_enter(func);
}

void __cyg_profile_func_exit(void *func, void *caller) {
	trace_block_func_exit(func);
}

