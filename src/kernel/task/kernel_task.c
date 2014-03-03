/**
 * @file
 *
 * @date Nov 12, 2013
 * @author: Anton Bondarev
 */

#include <embox/unit.h>
#include <kernel/task.h>
#include <kernel/task/task_table.h>
#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>

EMBOX_UNIT_INIT(kernel_task_init);

static struct task kernel_task
		__attribute__((section(".bss..reserve.ktask")));

struct task * task_kernel_task(void) {
	return &kernel_task;
}

static int kernel_task_init(void) {
	int ktask_id;

	ktask_id = task_table_add(task_kernel_task());
	if (ktask_id < 0) {
		return ktask_id;
	}

	task_init(task_kernel_task(), ktask_id, "kernel",
			cpu_get_idle(cpu_get_id()), TASK_PRIORITY_DEFAULT);

	return 0;
}
