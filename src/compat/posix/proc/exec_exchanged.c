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

	task = task_self();
	path = task_resource_argv_path(task);

	res = exec_call(path, NULL, NULL);

	return (void*)res;
}

int execv(const char *path, char *const argv[]) {
	struct task *task;
	struct task_param param;

	task = task_self();

	if (task_is_vforking(task)) {
		param.path = (char *)path;
		param.argv = (char **)argv;
		param.argc = argv_to_argc(argv);
		task_resource_exec(task, path, argv);
		vfork_child_done(task, task_stub_execv, &param);
	}

	return 0;
}

