/**
 * @file
 * @brief TODO
 *
 * @date 18.03.2012
 * @author Alina Kramar
 */

#ifndef PROFILER_TRACING_TRACE_H_
#define PROFILER_TRACING_TRACE_H_

#include __impl_x(profiler/tracing/trace_impl.h)

#define TRACE_POINT_DEF(name, tp_name) \
	  __TRACE_POINT_DEF(name, tp_name)

#define TRACE_BLOCK_DEF(tb) \
	  __TRACE_BLOCK_DEF(tb)

extern void trace_block_enter(struct __trace_block *tb);

extern void trace_block_leave(struct __trace_block *tb);

#define trace_point_set(tp_pointer) \
	  __trace_point_set(tp_pointer)

extern int trace_block_diff(struct __trace_block *tb);

#define trace_point(name) \
	  __trace_point(name)

#define trace_point_get_name(tp_pointer)  \
      __trace_point_get_name(tp_pointer)

#define trace_point_get_value(tp_pointer) \
      __trace_point_get_value(tp_pointer)

#endif /* PROFILER_TRACING_TRACE_H_ */
