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

#define tracepoint(name) \
	  __tracepoint(name)

#endif /* PROFILER_TRACING_TRACE_H_ */
