/**
 * @file
 * @brief TODO
 *
 * @date 18.03.2012
 * @author Alina Kramar
 */

#include <string.h>

#include <util/array.h>
#include <profiler/tracing/trace.h>

ARRAY_SPREAD_DEF_TERMINATED(typeof(struct __trace_point *),
		__trace_points_array, NULL);
ARRAY_SPREAD_DEF_TERMINATED(typeof(struct __trace_block *),
		__trace_blocks_array, NULL);

void __trace_block_enter(struct __trace_block *tb) {

	if (trace_point_get_value((tb)->begin) == 0) {
		trace_point_set((tb)->begin);
	}

	__tracepoint_handle((tb)->begin);
}

void __trace_block_leave(struct __trace_block *tb) {
	if (trace_point_get_value((tb)->end) == 0) {
		trace_point_set((tb)->end);
	}

	__tracepoint_handle((tb)->end);
}

int __trace_block_diff(struct __trace_block *tb) {
	int a = trace_point_get_value((tb)->begin);
	int b = trace_point_get_value((tb)->end);

	return a - b;
}

