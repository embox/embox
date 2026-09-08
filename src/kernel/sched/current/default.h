/**
 * @file
 * @brief
 *
 * @date 28.07.14
 * @author Vita Loginova
 */

#ifndef KERNEL_SCHEDEE_CURRENT_DEFAULT_H_
#define KERNEL_SCHEDEE_CURRENT_DEFAULT_H_

#include <hal/ipl.h>
#include <kernel/cpu/cpudata.h>

struct schedee;

extern struct schedee *__current_schedee;

/* See thread_self() in kernel/thread/core.c -- the same read and the same
 * race. Kept a macro rather than a static inline because at -O0 the latter is
 * a real call, and this is on every wait, wakeup and lock in the kernel.
 *
 * __schedee_set_current() below needs no mask: it is called from __schedule()
 * and sched_set_current(), both of which hold the BKL. */
#define __schedee_get_current() \
	({                                                 \
		struct schedee *__s;                           \
		ipl_t __ipl = ipl_save();                      \
		__s = cpudata_var(__current_schedee);          \
		ipl_restore(__ipl);                            \
		__s;                                           \
	})

#define __schedee_set_current(schedee) \
	do { cpudata_var(__current_schedee) = schedee; } while (0)

#endif /* KERNEL_SCHEDEE_CURRENT_DEFAULT_H_ */
