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

struct __tp {
	struct location_func location;
	int count;
	const char *name;
};

extern struct __tp * const __trace_points_array[];

#define __TRACE_POINT_DEF(_name, tp_name) \
		struct __tp _name = {                                 \
			.location = LOCATION_FUNC_INIT,                   \
			.name = tp_name,                                  \
			.count = 0,                                       \
		};                                                    \
		ARRAY_SPREAD_ADD(__trace_points_array, &_name)

static inline void __tracepoint_handle(struct __tp *p) {
	p->count++;
}

#define __tp_ref(__name) \
	({                                                        \
		static struct __tp __tp = {                           \
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

#define __trace_point_get_name(tp)             \
	({                                         \
		(tp)->name;                            \
	})

#define __trace_point_get_value(tp)            \
	({                                         \
		(tp)->count;                           \
	})

#endif /* PROFILER_TRACING_TRACE_IMPL_H_ */
