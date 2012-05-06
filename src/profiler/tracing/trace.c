/**
 * @file
 * @brief TODO
 *
 * @date 18.03.2012
 * @author Alina Kramar
 */

#include <string.h>

#include <util/array.h>
#include <util/location.h>

#include <profiler/tracing/trace.h>

ARRAY_SPREAD_DEF_TERMINATED(typeof(struct __trace_point *),
		__trace_points_array, NULL);
ARRAY_SPREAD_DEF_TERMINATED(typeof(struct __trace_block *),
		__trace_blocks_array, NULL);

void __tracepoint_handle(struct __trace_point *tp) {
	tp->count++;
}

void __trace_block_enter(struct __trace_block *tb) {
	__tracepoint_handle(tb->begin);
}

void __trace_block_leave(struct __trace_block *tb) {
	__tracepoint_handle(tb->end);
}

int __trace_block_diff(struct __trace_block *tb) {
	int a = trace_point_get_value(tb->begin);
	int b = trace_point_get_value(tb->end);

	return a - b;
}

const char *trace_point_get_name(struct __trace_point *tp) {
	return tp->name;
}

int trace_point_get_value(struct __trace_point *tp) {
	return tp->count;
}

