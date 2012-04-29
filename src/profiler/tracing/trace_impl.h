/**
 * @file
 * @brief TODO
 *
 * @date 18.03.2012
 * @author Alina Kramar
 */

#ifndef PROFILER_TRACING_TRACE_IMPL_H_
#define PROFILER_TRACING_TRACE_IMPL_H_

#include <string.h>

#include <util/array.h>
#include <util/location.h>

struct __trace_point {
	struct location_func location;
	int count;
	const char *name;
};

struct __trace_block {
	struct __trace_point *begin;
	struct __trace_point *end;
};

extern struct __trace_point * const __trace_points_array[];
extern struct __trace_block * const __trace_blocks_array[];

#define __TRACE_POINT_DEF(_name, tp_name) \
		struct __trace_point _name = {                                 \
			.location = LOCATION_FUNC_INIT,                   \
			.name = tp_name,                                  \
			.count = 0,                                       \
		};                                                    \
		ARRAY_SPREAD_ADD(__trace_points_array, &_name)

static inline void __tracepoint_handle(struct __trace_point *p) {
	p->count++;
}

#define __TRACE_BLOCK_DEF(tb_name)                           \
	static struct __trace_point b;             \
	static struct __trace_point e;             \
	static struct __trace_block tb_name  = {         \
			.begin = &b,                             \
			.end   = &e,                             \
	};                                                   \
	ARRAY_SPREAD_ADD(__trace_blocks_array, &tb_name);  \

#define __trace_block_enter(tb_pointer) \
	({ \
			if (trace_point_get_value((tb_pointer)->begin) == 0) { \
				trace_point_set((tb_pointer)->begin); \
			} \
			__tracepoint_handle((tb_pointer)->begin);\
	})

#define __trace_block_leave(tb_pointer) \
	({ \
			if (trace_point_get_value((tb_pointer)->end) == 0) { \
				trace_point_set((tb_pointer)->end); \
			} \
			__tracepoint_handle((tb_pointer)->end);                              \
	})

#define __trace_block_diff(tb_pointer) \
	({ \
		int a = trace_point_get_value((tb_pointer)->begin);     \
		int b = trace_point_get_value((tb_pointer)->end);       \
		a - b;                                                \
	})

#define __tp_ref(__name) \
	({                                                        \
		static struct __trace_point __tp = {                           \
			.location = LOCATION_FUNC_INIT,                   \
			.name = __name,                                   \
			.count = 0,                                       \
		};                                                    \
		ARRAY_SPREAD_ADD(__trace_points_array, &__tp);        \
		&__tp;                                                \
	})

#define __trace_point(__name) \
	__tracepoint_handle(__tp_ref(__name))

#define __trace_point_set(tp) \
	__tracepoint_handle(tp)

#define __trace_point_get_name(tp)                            \
	({                                                        \
		(tp)->name;                                           \
	})

#define __trace_point_get_value(tp)                           \
	({                                                        \
		(tp)->count;                                          \
	})

#endif /* PROFILER_TRACING_TRACE_IMPL_H_ */
