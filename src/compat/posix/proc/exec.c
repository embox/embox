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
#include <hal/vfork.h>

static const char * exec_cmd_name(const char *path) {
	size_t path_len;

	if (!strcmp(path, "/bin/sh")) {
		return "tish";
	}

	path_len = strlen(path);
	if (path_len >= strlen(".lua")
			&& !strcmp(path + path_len - strlen(".lua"), ".lua")) {
		return "lua";
	}

	return path;
}

int exec_call(void) {
	int ecode;
	struct task *task = task_self();
	const char *path = task_resource_argv_path(task);
	const char *cmd_name = exec_cmd_name(path);
	const struct shell *sh = shell_lookup(cmd_name);
	int c;
	char **v;

	if (strcmp(cmd_name, path))
		task_resource_argv_insert(task, cmd_name, 0);

	c = task_resource_argv_argc(task);
	v = task_resource_argv_argv(task);

	/* FIXME pass argv to shell_exec */
	if (sh) {
		ecode = shell_run(sh);
	} else {
		const struct cmd *cmd;

		cmd = cmd_lookup(cmd_name);

		if (cmd) {
			task_self_module_ptr_set(cmd2mod(cmd));
			ecode = cmd_exec(cmd, c, v);
		} else {
			ecode = ENOENT;
		}
	}

	return ecode;
}

int execv(const char *path, char *const argv[]) {
	struct task *task;
	/* save starting arguments for the task */
	task = task_self();
	task_resource_exec(task, path, argv);
	task_set_name(task, path);

	/* If vforked then unblock parent and start execute new image */
	vfork_child_done(task, task_exec_callback, NULL);

	/* Not vforked */
	exec_call();

	return 0;
}

int execve(const char *path, char *const argv[], char *const envp[]) {
	return execv(path, argv);
}

