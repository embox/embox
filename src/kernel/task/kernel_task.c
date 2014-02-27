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
#include <kernel/task/resource.h>
#include <string.h>
#include <util/dlist.h>

EMBOX_UNIT_INIT(kernel_task_init);

static struct task kernel_task
		__attribute__((section(".bss..reserve.ktask")));

struct task * task_kernel_task(void) {
	return &kernel_task;
}

static int kernel_task_init(void) {
	struct task *task;

	task = task_init(task_kernel_task(),
			sizeof *task + TASK_RESOURCE_SIZE,
			"kernel");
	if (task == NULL) {
		return -ENOMEM;
	}
	assert(task == task_kernel_task());

	dlist_init(&task->task_link); /* it's out list handler */

	return 0;
}
