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

/* int execve(const char *path, char *const argv[], char *const envp[]) {
	return execv(path, argv);
} */
