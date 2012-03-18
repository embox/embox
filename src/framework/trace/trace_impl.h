/**
 * @file
 * @brief TODO
 *
 * @date 18.03.2012
 * @author Alina Kramar
 */

#ifndef FRAMEWORK_TRACE_TRACE_IMPL_H_
#define FRAMEWORK_TRACE_TRACE_IMPL_H_

#include <string.h>

#include <util/array.h>
#include <util/location.h>

struct __tp {
	struct location_func location;
	int count;
	const char *name;
};

extern struct __tp * const __tracepoints_array[];

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
		ARRAY_SPREAD_ADD(__tracepoints_array, &__tp);         \
		&__tp;                                                \
	})

#define __tracepoint(__name) \
	__tracepoint_handle(__tp_ref(__name))

#endif /* FRAMEWORK_TRACE_TRACE_IMPL_H_ */
