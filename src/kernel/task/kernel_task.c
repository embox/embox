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

static struct task kernel_task __attribute__((section(".bss..reserve.ktask")));

struct task * task_kernel_task(void) {
	return &kernel_task;
}

static int kernel_task_init(void) {
	int ktask_id;
	struct task *t;
	//struct thread *idle_thread; /* main thread for kernel task */

	t = task_kernel_task();
	assert(t);

	ktask_id = task_table_add(t);
	if (ktask_id < 0) {
		return ktask_id;
	}

	task_init(t, ktask_id, "kernel", NULL, TASK_PRIORITY_DEFAULT);

	return 0;
}
