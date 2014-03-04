/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 25.02.14
 */

#ifndef KERNEL_TASK_RESOURCE_SIG_TABLE_H_
#define KERNEL_TASK_RESOURCE_SIG_TABLE_H_

#include <kernel/task.h>
#include <sys/cdefs.h>

struct task;
struct sigaction;

__BEGIN_DECLS

extern struct sigaction * task_resource_sig_table(
		const struct task *task);

#define task_self_resource_sig_table() \
	task_resource_sig_table(task_self())

__END_DECLS

#endif /* KERNEL_TASK_RESOURCE_SIG_TABLE_H_ */
