/**
 * @file
 *
 * @date Nov 12, 2013
 * @author: Anton Bondarev
 */

#include <embox/unit.h>
#include <kernel/task.h>
#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>

EMBOX_UNIT_INIT(kernel_task_init);

static struct task kernel_task
		__attribute__((section(".bss..reserve.ktask")));

struct task * task_kernel_task(void) {
	return &kernel_task;
}

static int kernel_task_init(void) {
	task_init(&kernel_task, 0, "kernel",
			cpu_get_idle(cpu_get_id()), TASK_PRIORITY_DEFAULT);

	return 0;
}
