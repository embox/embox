/*
 * @file
 *
 * @date Aug 29, 2012
 * @author: Anton Bondarev
 */

#ifndef NO_TRACE_IMPL_H_
#define NO_TRACE_IMPL_H_

struct __trace_point;

struct __trace_block {
	int proxy;
};

#define __TRACE_POINT_DEF(name, tp_name)
#define __TRACE_BLOCK_DEF(tb) struct __trace_block tb;
#define __trace_point_set(tp_pointer)
#define __trace_point(__name)

static inline void __tracepoint_handle(struct __trace_point *tp){}

static inline void trace_block_enter(struct __trace_block *tb) {};

static inline void trace_block_leave(struct __trace_block *tb) {};

#endif /* NO_TRACE_IMPL_H_ */
