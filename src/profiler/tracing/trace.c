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

#include <kernel/clock_source.h>
#include <kernel/ktime.h>
#include <kernel/time/timecounter.h>

#include <profiler/tracing/trace.h>

ARRAY_SPREAD_DEF_TERMINATED(typeof(struct __trace_point *),
		__trace_points_array, NULL);
ARRAY_SPREAD_DEF_TERMINATED(typeof(struct __trace_block *),
		__trace_blocks_array, NULL);

void __tracepoint_handle(struct __trace_point *tp) {
	tp->count++;
}

void trace_block_enter(struct __trace_block *tb) {
	if (tb->active) {
		timecounter_init(tb->tc, clock_source_get_default()->cc, 0);
		__tracepoint_handle(tb->begin);
	}
}

void trace_block_leave(struct __trace_block *tb) {
	if (tb->active) {
		tb->time = (int) timecounter_read(tb->tc);
		__tracepoint_handle(tb->end);
	}
}

int trace_block_get_time(struct __trace_block *tb) {
	return tb->time;
}

int trace_block_diff(struct __trace_block *tb) {
	int a = trace_point_get_value(tb->begin);
	int b = trace_point_get_value(tb->end);

	return a - b;
}

int trace_point_get_value(struct __trace_point *tp) {
	return tp->count;
}

