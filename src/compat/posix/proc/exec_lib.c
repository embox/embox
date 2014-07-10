/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

#include <string.h>
#include <framework/cmd/api.h>
#include <cmd/shell.h>

#include <kernel/task/resource/module_ptr.h>

#include <kernel/task.h>
#include <kernel/task/resource/task_argv.h>
#include <kernel/task/resource.h>


int exec_call(void) {
	int ecode;
	struct task *task = task_self();
	int c = task_resource_argv_argc(task);
	char **v = task_resource_argv_argv(task);
	const char *path = task_resource_argv_path(task);
	const struct shell *sh = shell_lookup(path);

	if (!sh) {
		if (!strcmp(path, "/bin/sh")) {
			sh = shell_lookup("tish");
		}
	}

	/* FIXME pass argv to shell_exec */
	if (sh) {
		ecode = shell_run(sh);
	} else {
		const struct cmd *cmd;

		cmd = cmd_lookup(path);

		if (cmd) {
			task_self_module_ptr_set(cmd2mod(cmd));
			ecode = cmd_exec(cmd, c, v);
		} else {
			ecode = ENOENT;
		}
	}

	return ecode;
}

extern void vfork_child_done(struct task *child, void * (*run)(void *));
extern void *task_exit_callback(void *arg);
extern void *task_exec_callback(void *arg);

int execv(const char *path, char *const argv[]) {
	struct task *task;
	/* save starting arguments for the task */
	task = task_self();
	task_resource_exec(task, path, argv);

	/* if vforked then unblock parent and  start execute new image */
	vfork_child_done(task, task_exec_callback);

	return 0;
}

int execve(const char *path, char *const argv[], char *const envp[]) {
	return execv(path, argv);
}

