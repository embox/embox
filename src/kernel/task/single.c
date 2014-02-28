/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <kernel/task.h>

int new_task(const char *name, void *(*run)(void *), void *arg) {
	return -EPERM;
}

struct task *task_self(void) {
	/* Since there is only one task, actually it means --
	 * that task is kernel's.
	 */
	return task_kernel_task();
}

void __attribute__((noreturn)) task_exit(void *res) {
	while(1);
}

int task_notify_switch(struct thread *prev, struct thread *next) {
	return 0;
}

short task_get_priority(struct task *tsk) {
	return 0;
}

int task_set_priority(struct task *tsk, task_priority_t new_priority) {
	return 0;
}
