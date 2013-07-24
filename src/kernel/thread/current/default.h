/**
 * @file
 * @brief
 *
 * @date 13.02.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_CURRENT_DEFAULT_H_
#define KERNEL_THREAD_CURRENT_DEFAULT_H_

#include <kernel/cpu/cpudata.h>

struct thread;

extern struct thread *__current_thread;

#define __thread_get_current() \
	cpudata_var(__current_thread)

#define __thread_set_current(thread) \
	do { cpudata_var(__current_thread) = thread; } while (0)

#endif /* KERNEL_THREAD_CURRENT_DEFAULT_H_ */
