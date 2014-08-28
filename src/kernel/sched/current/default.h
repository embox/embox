/**
 * @file
 * @brief
 *
 * @date 28.07.14
 * @author Vita Loginova
 */

#ifndef KERNEL_SCHEDEE_CURRENT_DEFAULT_H_
#define KERNEL_SCHEDEE_CURRENT_DEFAULT_H_

#include <kernel/cpu/cpudata.h>

struct schedee;

extern struct schedee *__current_schedee;

#define __schedee_get_current() \
	cpudata_var(__current_schedee)

#define __schedee_set_current(schedee) \
	do { cpudata_var(__current_schedee) = schedee; } while (0)

#endif /* KERNEL_SCHEDEE_CURRENT_DEFAULT_H_ */
