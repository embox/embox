/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <kernel/task.h>
#include <kernel/file.h>
#include "index_desc.h"

int task_create(struct task **new, struct task *parent) {
	*new = parent;
	return 0;
}

struct task *task_self(void) {
	return task_default_get();
}

