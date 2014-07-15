/**
 * @file
 *
 * @date Nov 12, 2013
 * @author: Anton Bondarev
 */
#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/task_table.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(kernel_task_init);

static struct task kernel_task __attribute__((section(".bss.ktask")));

struct task * task_kernel_task(void) {
	return &kernel_task;
}

static int kernel_task_init(void) {
	int ktask_id;
	struct task *ktask;

	ktask = task_kernel_task();
	assert(ktask != NULL);

	ktask_id = task_table_add(ktask);
	if (ktask_id < 0) {
		return ktask_id;
	}

	/* task_get_main returns a value which already initialized
	 * by thread.core module in the file idle_thread.c by the
	 * idle_thread_create() function */
	task_init(ktask, ktask_id, NULL, "kernel", task_get_main(ktask),
			TASK_PRIORITY_DEFAULT);

	return 0;
}
