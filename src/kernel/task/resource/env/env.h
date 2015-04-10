/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 30.03.13
 */

#ifndef KERNEL_TASK_RESOURCE_ENV_H_
#define KERNEL_TASK_RESOURCE_ENV_H_

#include <kernel/task.h>
#include <module/embox/kernel/task/resource/env.h>
#include <sys/cdefs.h>

struct task;
struct task_env;

__BEGIN_DECLS

extern struct task_env * task_resource_env(const struct task *task);

#define task_self_resource_env() task_resource_env(task_self())

__END_DECLS

#endif /* KERNEL_TASK_RESOURCE_ENV_H_ */
