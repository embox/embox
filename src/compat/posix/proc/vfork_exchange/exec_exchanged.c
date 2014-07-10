/**
 * @file
 *
 * @date May 21, 2014
 * @author: Anton Bondarev
 */
#include <stddef.h>

#include <cmd/shell.h>
#include <kernel/task.h>
#include <hal/vfork.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/task_argv.h>

extern int exec_call();

static void *task_stub_execv(void *arg) {
	int res;

	res = exec_call();

	return (void*)res;
}

int execv(const char *path, char *const argv[]) {
	struct task *task;

	task = task_self();
	task_resource_exec(task, path, argv);

	vfork_child_done(task, task_stub_execv);

	return 0;
}

