#include <stdio.h>
#include <profiler/tracing/trace.h>
#include <embox/unit.h>
#include "cyg_profile.h"

ARRAY_SPREAD_DEF_TERMINATED(cyg_func, __cyg_handler_enter_array, NULL);
ARRAY_SPREAD_DEF_TERMINATED(cyg_func, __cyg_handler_exit_array, NULL);

void __cyg_profile_func_enter(void *func, void *caller) {
	void (*hnd)(void *, void*);

	array_spread_nullterm_foreach(hnd, __cyg_handler_enter_array) {
		(*hnd)(func, caller);
	}
}

void __cyg_profile_func_exit(void *func, void *caller) {
	void (*hnd)(void *, void *);

	array_spread_nullterm_foreach(hnd, __cyg_handler_exit_array) {
		(*hnd)(func, caller);
	}
}
