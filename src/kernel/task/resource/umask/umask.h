/**
 * @file
 *
 * @date Dec 21, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_KERNEL_TASK_RESOURCE_UMASK_UMASK_H_
#define SRC_KERNEL_TASK_RESOURCE_UMASK_UMASK_H_

#include <sys/types.h>

#include <kernel/task.h>
#include <sys/cdefs.h>

struct task;

__BEGIN_DECLS

extern mode_t *task_resource_umask(const struct task *task);

#define task_self_resource_umask() \
	task_resource_umask(task_self())

__END_DECLS

#endif /* SRC_KERNEL_TASK_RESOURCE_UMASK_UMASK_H_ */
