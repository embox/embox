/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 25.02.14
 */

#ifndef KERNEL_TASK_RESOURCE_IDESC_TABLE_H_
#define KERNEL_TASK_RESOURCE_IDESC_TABLE_H_

#include <kernel/task.h>
#include <module/embox/kernel/task/resource/idesc_table.h>
#include <sys/cdefs.h>

struct task;
struct idesc_table;

__BEGIN_DECLS

extern struct idesc_table * task_resource_idesc_table(
		const struct task *task);

#define task_self_resource_idesc_table() \
	task_resource_idesc_table(task_self())

__END_DECLS

#endif /* KERNEL_TASK_RESOURCE_IDESC_TABLE_H_ */
