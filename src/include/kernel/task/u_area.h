/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.02.2013
 */

#ifndef KERNEL_TASK_U_AREA_H_
#define KERNEL_TASK_U_AREA_H_

#include <sys/types.h>
#include <kernel/task.h>

struct task_u_area {
	uid_t	reuid;
	uid_t   euid;
	gid_t	regid;
	gid_t	egid;
};

static inline struct task_u_area *task_self_u_area(void) {
	return task_self()->u_area;
}

#endif /* KERNEL_TASK_U_AREA_H_ */
