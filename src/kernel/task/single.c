/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <kernel/task.h>
#include <kernel/task_resources.h>

#include "index_desc.h"

int task_create(struct task **new, struct task *parent) {
	return -EPERM;
}

struct task *task_self(void) {
	/* Since there is no task, actually it means there are only
	 * one task -- kernel task
	 */
	return task_kernel_task();
}

