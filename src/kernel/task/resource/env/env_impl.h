/**
 * @file   env_impl.h
 * @brief  Implementation of struct task_env
 *
 * @author Ilia Vaprol
 * @date 30.03.13
 */

#ifndef ENV_H_
#define ENV_H_

#include <config/embox/kernel/task/resource/env.h>
#include <framework/mod/options.h>
#include <kernel/task/resource/env.h>
#include <stddef.h>

#define MODOPS_ENV_PER_TASK \
	OPTION_MODULE_GET(embox__kernel__task__resource__env, \
			NUMBER, env_per_task)
#define MODOPS_ENV_STR_LEN \
	OPTION_MODULE_GET(embox__kernel__task__resource__env, \
			NUMBER, env_str_len)

struct task_env {
	char **envs;
	size_t next;
	char *vals[MODOPS_ENV_PER_TASK + 1];
	char storage[MODOPS_ENV_PER_TASK][MODOPS_ENV_STR_LEN];
	char buff[MODOPS_ENV_STR_LEN];
};

#endif /* ENV_H_ */
