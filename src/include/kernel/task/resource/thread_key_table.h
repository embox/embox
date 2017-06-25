/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 25.02.14
 */

#ifndef KERNEL_TASK_RESOURCE_THREAD_KEY_TABLE_H_
#define KERNEL_TASK_RESOURCE_THREAD_KEY_TABLE_H_

#include <kernel/task.h>
#include <module/embox/kernel/task/resource/thread_key_table.h>
#include <sys/cdefs.h>

struct task;
struct thread_key_table;

__BEGIN_DECLS

extern struct thread_key_table * task_resource_thread_key_table(
		const struct task *task);

#define task_self_resource_idesc_table() \
	task_resource_idesc_table(task_self())

__END_DECLS

#endif /* KERNEL_TASK_RESOURCE_THREAD_KEY_TABLE_H_ */
