/**
 * @file
 *
 * @date Nov 12, 2013
 * @author: Anton Bondarev
 */
#include "common.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <kernel/task.h>

#include <module/embox/kernel/task/api.h>

#include <embox/unit.h>


EMBOX_UNIT_INIT(kernel_task_init);

#define MAX_RES_SUM_SIZE \
	OPTION_MODULE_GET(embox__kernel__task__api, NUMBER, max_resource_size)

struct kernel_task {
	struct task task;
	char resource[MAX_RES_SUM_SIZE];
};

static struct kernel_task kernel_task;

struct task *task_kernel_task(void) {
	return &kernel_task.task;
}


extern void resource_sum_size_calc(void);

extern char **environ;

static int kernel_task_init(void) {
	struct task *task;
	resource_sum_size_calc();

	task = task_kernel_task();

	environ = *task_self_environ_ptr();

	if (!task_init(task, sizeof(kernel_task))) {
		return -ENOMEM;
	}

	dlist_init(&task->task_link); /* it's out list handler */

	strncpy(task->task_name, "kernel", sizeof(task_kernel_task()->task_name) - 1);

	return 0;
}
