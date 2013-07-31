/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 30.03.13
 */

#ifndef KERNEL_TASK_ENV_H_
#define KERNEL_TASK_ENV_H_

#include <framework/mod/options.h>
#include <stddef.h>
#include <sys/cdefs.h>

#include <kernel/task.h>

#include <module/embox/kernel/task/env.h>

#define MODOPS_ENV_PER_TASK \
	OPTION_MODULE_GET(embox__kernel__task__env, NUMBER, env_per_task)

#define MODOPS_ENV_STR_LEN \
	OPTION_MODULE_GET(embox__kernel__task__env, NUMBER, env_str_len)

__BEGIN_DECLS

struct task_env {
	char **envs;
	size_t next;
	char *vals[MODOPS_ENV_PER_TASK + 1];
	char storage[MODOPS_ENV_PER_TASK][MODOPS_ENV_STR_LEN];
	char buff[MODOPS_ENV_STR_LEN];
};

static inline struct task_env * task_self_env(void) {
	return task_self()->env;
}

__END_DECLS

#endif /* KERNEL_TASK_ENV_H_ */
