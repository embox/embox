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

extern int exec_call(char *path, char *argv[], char *envp[]);

static void *task_stub_execv(void *arg) {
	struct task_param *param = arg;
	int res;

	res = shell_exec(shell_any(), param->path);
	res = exec_call(param->path, NULL, NULL);

	return (void*)res;
}

int execv(const char *path, char *const argv[]) {
	struct task *task;
	struct task_param *param = {0};

	task = task_self();

	if (task_is_vforking(task)) {
		param->path = (char *)path;
		param->argv = (char **) argv;
		vfork_child_done(task, task_stub_execv, param);
	}

	return 0;
}

