#include <stdio.h>
#include <profiler/tracing/trace.h>

void __cyg_profile_func_enter(void *func, void *caller) {
	/* This function is for instrument profiling. It is being called just before
	 * every call of instrumented funcion.
	 * You can try to get more info by searching for "-finstrument-functions" GCC flag
	 */
	if (cyg_profiling) {
		cyg_profiling = false;
		trace_block_func_enter(func);
		cyg_profiling = true;
	}
}

void __cyg_profile_func_exit(void *func, void *caller) {
	/* This function is for instrument profiling. It is being called after every
	 * exit from instrumented funcion.
	 * You can try to get more info by searching for "-finstrument-functions" GCC flag
	 */
	 if (cyg_profiling) {
		cyg_profiling = false;
		trace_block_func_exit(func);
		cyg_profiling = true;
	}
}

