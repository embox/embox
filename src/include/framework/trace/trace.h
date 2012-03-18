/**
 * @file
 * @brief TODO
 *
 * @date 18.03.2012
 * @author Alina Kramar
 */

#ifndef FRAMEWORK_TRACE_TRACE_H_
#define FRAMEWORK_TRACE_TRACE_H_

#include __impl_x(framework/trace/trace_impl.h)

#define tracepoint(name) \
	  __tracepoint(name)

#endif /* FRAMEWORK_TRACE_TRACE_H_ */
