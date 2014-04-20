#include <stdio.h>
#include <profiler/tracing/trace.h>

void __cyg_profile_func_enter(void *func, void *caller) {
	/* This function is for instrument profiling. It is being called just before
	 * every call of instrumented funcion.
	 * You can try to get more info by searching for "-finstrument-functions" GCC flag
	 */
	if (get_profiling_mode() == CYG_PROFILING) {
		set_profiling_mode(DISABLED);
		trace_block_func_enter(func);
		set_profiling_mode(CYG_PROFILING);
	}
}

void __cyg_profile_func_exit(void *func, void *caller) {
	/* This function is for instrument profiling. It is being called after every
	 * exit from instrumented funcion.
	 * You can try to get more info by searching for "-finstrument-functions" GCC flag
	 */
	 if (get_profiling_mode() == CYG_PROFILING) {
		set_profiling_mode(DISABLED);
		trace_block_func_exit(func);
		set_profiling_mode(CYG_PROFILING);
	}
}

