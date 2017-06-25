/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.02.2013
 */

#ifndef KERNEL_TASK_RESOURCE_U_AREA_H_
#define KERNEL_TASK_RESOURCE_U_AREA_H_

#include <kernel/task.h>
#include <sys/cdefs.h>
#include <sys/types.h>

struct task;

struct task_u_area {
	uid_t	reuid;
	uid_t   euid;
	gid_t	regid;
	gid_t	egid;
};

__BEGIN_DECLS

extern struct task_u_area * task_resource_u_area(
		const struct task *task);

#define task_self_resource_u_area() \
	task_resource_u_area(task_self())

__END_DECLS

#endif /* KERNEL_TASK_RESOURCE_U_AREA_H_ */
