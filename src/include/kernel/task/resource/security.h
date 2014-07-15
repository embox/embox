/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 26.02.14
 */

#ifndef KERNEL_TASK_RESOURCE_SECURITY_H_
#define KERNEL_TASK_RESOURCE_SECURITY_H_

#include <kernel/task.h>
#include <sys/cdefs.h>

struct task;

__BEGIN_DECLS

extern void * task_resource_security(const struct task *task);

#define task_self_resource_security() \
	task_resource_security(task_self())

__END_DECLS

#endif /* KERNEL_TASK_RESOURCE_SECURITY_H_ */
