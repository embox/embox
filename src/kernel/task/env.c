/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 30.03.13
 */

#include <kernel/task.h>
#include <kernel/task/env.h>

struct env_struct * task_self_env_ptr(void) {
	return &task_self()->env;
}

char *** task_self_environ_ptr(void) {
	return &task_self()->env.env;
}
