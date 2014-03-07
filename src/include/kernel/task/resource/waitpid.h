/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.02.2013
 */

#ifndef KERNEL_TASK_RESOURCE_WAITPID_H_
#define KERNEL_TASK_RESOURCE_WAITPID_H_

#include <kernel/task.h>
#include <sys/cdefs.h>

struct task;
struct waitq;

__BEGIN_DECLS

extern struct waitq * task_resource_waitpid(
		const struct task *task);

#define task_self_resource_waitpid() \
	task_resource_waitpid(task_self())

__END_DECLS

#endif /* KERNEL_TASK_RESOURCE_WAITPID_H_ */
