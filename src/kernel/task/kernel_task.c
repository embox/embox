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
#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>

EMBOX_UNIT_INIT(kernel_task_init);

static struct task kernel_task
		__attribute__((section(".bss..reserve.ktask")));

struct task * task_kernel_task(void) {
	return &kernel_task;
}

static int kernel_task_init(void) {
	struct task *tsk;

	tsk = task_init(task_kernel_task(),
			sizeof *tsk + TASK_RESOURCE_SIZE,
			0, "kernel", cpu_get_idle(cpu_get_id()),
			TASK_PRIORITY_DEFAULT);
	if (tsk == NULL) {
		return -ENOMEM;
	}
	assert(tsk == task_kernel_task());

	return 0;
}
