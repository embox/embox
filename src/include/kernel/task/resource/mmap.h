/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 26.02.14
 */

#ifndef KERNEL_TASK_RESOURCE_MMAP_H_
#define KERNEL_TASK_RESOURCE_MMAP_H_

#include <kernel/task.h>
#include <sys/cdefs.h>

struct emmap;
struct task;

__BEGIN_DECLS

extern struct emmap * task_resource_mmap(const struct task *task);

#define task_self_resource_mmap() \
	task_resource_mmap(task_self())

__END_DECLS

#endif /* KERNEL_TASK_RESOURCE_MMAP_H_ */
