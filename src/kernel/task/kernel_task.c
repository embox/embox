/**
 * @file
 *
 * @date Nov 12, 2013
 * @author: Anton Bondarev
 */

#include <embox/unit.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <kernel/task.h>
#include <module/embox/kernel/task/task_resource.h>
#include <string.h>
#include <util/dlist.h>

#define MODOPS_TASK_RESOURCE_MAX_SIZE \
	OPTION_MODULE_GET(embox__kernel__task__task_resource, \
			NUMBER, task_resource_max_size)

EMBOX_UNIT_INIT(kernel_task_init);

static struct {
	struct task task;
	char __resource_storage[MODOPS_TASK_RESOURCE_MAX_SIZE];
} kernel_task;

struct task * task_kernel_task(void) {
	return &kernel_task.task;
}

static int kernel_task_init(void) {
	struct task *task;

	task = task_init(task_kernel_task(), sizeof kernel_task,
			"kernel");
	if (task == NULL) {
		return -ENOMEM;
	}
	assert(task == task_kernel_task());

	dlist_init(&task->task_link); /* it's out list handler */

	return 0;
}
