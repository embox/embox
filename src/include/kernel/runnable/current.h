/**
 * @file
 * @brief
 *
 * @date 28.07.14
 * @author Vita Loginova
 */

#ifndef KERNEL_RUNNABLE_CURRENT_DEFAULT_H_
#define KERNEL_RUNNABLE_CURRENT_DEFAULT_H_

#include <kernel/cpu/cpudata.h>

struct runnable;

extern struct runnable *__current_runnable;

#define runnable_get_current() \
	cpudata_var(__current_runnable)

#define runnable_set_current(runnable) \
	do { cpudata_var(__current_runnable) = runnable; } while (0)

#endif /* KERNEL_RUNNABLE_CURRENT_DEFAULT_H_ */
