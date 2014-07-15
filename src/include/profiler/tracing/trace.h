/**
 * @file
 * @brief TODO
 *
 * @date 18.03.2012
 * @author Alina Kramar
 */

#ifndef PROFILER_TRACING_TRACE_H_
#define PROFILER_TRACING_TRACE_H_

/* for time64_t */
#include <kernel/time/time.h>

struct __trace_point;
struct __trace_block;

#include <module/embox/profiler/trace.h>

#define TRACE_POINT_DEF(name, tp_name) \
	  __TRACE_POINT_DEF(name, tp_name)

#define TRACE_BLOCK_DEF(tb) \
	  __TRACE_BLOCK_DEF(tb)

#define trace_point(name) \
	  __trace_point(name)

#include <stdbool.h>

/* Different profiling mode are required to work separately with for example
 * manually assigned trace_blocks and automatically generated trace_blocks */
typedef enum {
	DISABLED,
	MANUAL_PROFILING,
	CYG_PROFILING,		/* Automatic instrumenting */
} profiling_mode;

extern profiling_mode 	get_profiling_mode(void);
extern void				set_profiling_mode(profiling_mode new_mode);

extern void __tracepoint_handle(struct __trace_point *tp);

extern void trace_block_enter(struct __trace_block *tb);

extern void trace_block_leave(struct __trace_block *tb);

#define trace_point_set(tp_pointer) \
		__trace_point_set(tp_pointer)

extern time64_t trace_block_diff(struct __trace_block *tb);

extern time64_t trace_block_get_time(struct __trace_block *tb);

extern int trace_point_get_value(struct __trace_point *tp);

extern struct __trace_point *trace_point_get_by_name(const char *name);

extern struct __trace_block *auto_profile_tb_first(void);

extern struct __trace_block *auto_profile_tb_next(struct __trace_block *prev);

extern void print_trace_block_info(struct __trace_block *tb);

extern void trace_block_func_enter(void *func);

extern void trace_block_func_exit(void *func);

extern long long get_current_tb_resolution(void);

extern void trace_block_hashtable_init(void);

extern void trace_block_hashtable_destroy(void);

#endif /* PROFILER_TRACING_TRACE_H_ */
