/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 30.03.13
 */

#include <assert.h>
#include <kernel/task.h>
#include <kernel/task/env.h>
#include <stddef.h>
#include <string.h>

char *** task_self_environ_ptr(void) {
	return &task_self()->env->envs;
}

static void env_init(struct task *task, void *env_) {
	struct task_env *env;

	assert(task);
	assert(env_);

	env = env_;
	env->envs = NULL;
	env->next = 0;

	task->env = env;
}

static int env_inherit(struct task *task, struct task *parent) {
	size_t i;

	assert(task);
	assert(parent);

	assert(task->env);
	assert(parent->env);

	task->env->envs = parent->env->envs != NULL ? &task->env->vals[0] : NULL;
	task->env->next = parent->env->next;
	for (i = 0; i < task->env->next; ++i) {
		task->env->vals[i] = &task->env->storage[i][0];
	}
	memcpy(&task->env->storage[0][0], &parent->env->storage[0][0],
			task->env->next * ARRAY_SIZE(task->env->storage[0]));
	return 0;
}

static void env_deinit(struct task *task) {
	assert(task);
	assert(task->env);

	task->env->envs = NULL;
	task->env->next = 0;
}

static const struct task_resource_desc env_res = {
	.init = env_init,
	.inherit = env_inherit,
	.deinit = env_deinit,
	.resource_size = sizeof(struct task_env),
};

TASK_RESOURCE_DESC(&env_res);
