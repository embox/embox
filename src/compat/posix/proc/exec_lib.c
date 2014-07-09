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

int exec_call() {
	const struct shell *sh;
	int ecode;
	struct task *task;
	int c;
	char **v;
	const char *path;

	task = task_self();
	c = task_resource_argv_argc(task);
	v = task_resource_argv_argv(task);
	path = task_resource_argv_path(task);
	sh = shell_lookup(path);

	if (!strncmp(path, "/bin/", strlen("/bin/"))) {
		path += strlen("/bin/");
	}

	if (!sh) {
		if (!strcmp(path, "sh")) {
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
