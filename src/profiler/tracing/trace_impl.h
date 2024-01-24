/**
 * @file
 * @brief TODO
 *
 * @date 18.03.2012
 * @author Alina Kramar
 * @author Denis Deryugin
 */

#ifndef PROFILER_TRACING_TRACE_IMPL_H_
#define PROFILER_TRACING_TRACE_IMPL_H_

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <lib/libds/array.h>
#include <util/location.h>

#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>
#include <kernel/time/itimer.h>

#include <embox/unit.h>

#define FUNC_QUANTITY OPTION_GET(NUMBER,max_functions_quantity)
#define TB_MAX_DEPTH  OPTION_GET(NUMBER,max_recursion_depth)

struct __trace_point {
	const char *name;
	struct location_func location;
	int count;
	bool active;
};

struct tb_time {
	time64_t time;
	struct tb_time *next;
};

struct __trace_block {
	const char *name;
	void *func;
	struct location_func location;

	/* List of all time_enter on recursive trace_block_enter */
	struct tb_time *time_list_head;

	int depth; /* Depth of recursion */

	time64_t time;
	time64_t max_time;

	int64_t count;
	bool is_entered;
	bool active;
};

#define __TRACE_POINT_DEF(_name, tp_name)   \
		struct __trace_point _name = {      \
			.name = tp_name,                \
			.location = LOCATION_FUNC_INIT, \
			.count = 0,                     \
			.active = true,                 \
		};                                  \
		ARRAY_SPREAD_DECLARE(struct __trace_point *, \
				__trace_points_array);               \
		ARRAY_SPREAD_ADD(__trace_points_array, &_name)

#define __trace_point_set(tp_pointer) \
		__tracepoint_handle(tp_pointer)

#define __TRACE_BLOCK_DEF(tb_name)                  	\
	static struct __trace_block tb_name  = {        	\
			.name  = #tb_name,							\
			.location = LOCATION_FUNC_INIT,				\
			.time  = 0,									\
			.max_time = 0,								\
			.count = 0,									\
			.depth = 0,									\
			.active = true, 							\
			.is_entered = false,						\
			.time_list_head = NULL,						\
	};                                              	\
	ARRAY_SPREAD_DECLARE(struct __trace_block *,		\
			__trace_blocks_array);              		\
	ARRAY_SPREAD_ADD(__trace_blocks_array, &tb_name)

#define __tp_ref(__name) \
	({                                                        \
		static struct __trace_point __tp = {                  \
			.name = __name,                                   \
			.location = LOCATION_FUNC_INIT,                   \
			.count = 0,                                       \
			.active = true,                                   \
		};                                                    \
		ARRAY_SPREAD_DECLARE(struct __trace_point *,          \
				__trace_points_array);                        \
		ARRAY_SPREAD_ADD(__trace_points_array, &__tp);        \
		&__tp;                                                \
	})

#define __trace_point(__name) \
	__tracepoint_handle(__tp_ref(__name))

#endif /* PROFILER_TRACING_TRACE_IMPL_H_ */
