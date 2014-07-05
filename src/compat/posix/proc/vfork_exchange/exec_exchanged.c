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

extern int exec_call(char *path, char *argv[], char *envp[]);

static void *task_stub_execv(void *arg) {
	struct task *task;
	int res;
	char *path;
	char **argv;

	task = task_self();

	path = task_resource_argv_path(task);
	argv = task_resource_argv_argv(task);

	res = exec_call(path, argv, NULL);

	return (void*)res;
}

int execv(const char *path, char *const argv[]) {
	struct task *task;

	task = task_self();

	if (task_is_vforking(task)) {
		task_resource_exec(task, path, argv);
		vfork_child_done(task, task_stub_execv);
	}

	// do execv if task is not forked?

	return 0;
}

