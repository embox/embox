/**
 * @file
 *
 * @date May 21, 2014
 * @author: Anton Bondarev
 */
#include <stddef.h>

#include <cmd/shell.h>
#include <kernel/task.h>
#include <kernel/task/resource/task_vfork.h>


extern int task_is_vforking(struct task *task);
extern void vfork_child_done(struct task *task, void * (*run)(void *), void *arg);

static void *task_stub_execv(void *arg) {
	struct task *task;
	struct task_vfork *task_vfork;
	int res;

	task = task_self();
	task_vfork = task_resource_vfork(task);

	res = shell_exec(shell_any(), task_vfork->cmdline);

	return (void*)res;
}

int execv(const char *path, char *const argv[]) {
	struct task *task;

	task = task_self();

	if (task_is_vforking(task)) {
		struct task_vfork *task_vfork;

		task_vfork = task_resource_vfork(task);
		strncpy(task_vfork->cmdline, path, sizeof(task_vfork->cmdline) - 1);

		vfork_child_done(task, task_stub_execv, NULL);
	}

	return 0;
}

