/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <kernel/task.h>

#include "index_desc.h"

int task_create(struct task **new, struct task *parent) {
	return -EPERM;
}

struct task *task_self(void) {
	/* Since there is only one task, actually it means --
	 * that task is kernel's.
	 */
	return task_kernel_task();
}

