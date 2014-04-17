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

#include <util/array.h>
#include <util/location.h>
#include <util/list.h>

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

	/* This array acts like a stack to capture multiple enter time when block
	 * is being entered recursively.
	 *
	 * "depth" field is the pointer to the top of the stack.
	 *
	 * It is not necessary to fill this array with anything
	 * as it acts like stack: normally uninitialized values
	 * are not to be used */

	struct tb_time *time_list_head;
	//time64_t start_time[TB_MAX_DEPTH];

	int depth;

	time64_t time;

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
