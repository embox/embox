/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <framework/cmd/api.h>
#include <cmd/shell.h>

#include <kernel/task/resource/module_ptr.h>

#include <kernel/task.h>
#include <kernel/task/resource/task_argv.h>
#include <kernel/task/resource.h>
#include <hal/vfork.h>

#include <errno.h>
#include <stdarg.h>

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
			ecode = -ENOENT;
			errno = ENOENT;
		}
	}

	return ecode;
}

int execv(const char *path, char *const argv[]) {
	struct task *task;
    const struct cmd *cmd;

    /* check whether a valid executable command name is given */
    const struct shell *sh = shell_lookup(path);
    if (!sh) {
        cmd = cmd_lookup(path);
        if (!cmd) {
            errno = ENOENT;
            return -1;
        }
    }

	task = task_self();
	task_resource_exec(task, path, argv);
	task_set_name(task, path);

#if 0
	int i;
	size_t len;
	char cmd_name[MAX_TASK_NAME_LEN];
	/* save starting arguments for the task */
	task = task_self();
	task_resource_exec(task, path, argv);
	cmd_name[0] = '\0';
	if (argv != NULL) {
        /* number of arguments constrained by 3*/
        for(i=0; argv[i]!=NULL && i<4; i++){
            len = strlen(cmd_name);
            if(MAX_TASK_NAME_LEN-len-1 <= 0)
                break;
            if(i>0) /* No space in the begining */
                strncat(cmd_name, " ", MAX_TASK_NAME_LEN-len-1);
			strncat(cmd_name, argv[i], MAX_TASK_NAME_LEN-len-1);
		}
	} else {
		strncpy(cmd_name, path, MAX_TASK_NAME_LEN - 1);
		cmd_name[MAX_TASK_NAME_LEN - 1] = '\0';
	}
	task_set_name(task, cmd_name);
#endif	

	/* If vforked then unblock parent and start execute new image */
	vfork_child_done(task, task_exec_callback, NULL);

	/* Not vforked */
	int ecode = exec_call();

	if (ecode == 0) {
		task_exit(0);
	}
	return -1;

}

static int switch_env(char *const envp[]) {
	int rc = 0;

	if (!envp)
		return 0;

	clearenv();

	while (*envp != NULL) {
		rc = putenv(*envp);
		if (rc != 0)
			return rc;
		envp++;
	}

	return 0;
}

int execve(const char *path, char *const argv[], char *const envp[]) {
	int rc = 0;

	rc = switch_env(envp);
	if (rc != 0)
		return rc;

	return execv(path, argv);
}

int execl(const char *path, const char *arg, ...) {
	char *buf[16];
	int i;
	va_list args;
	
	*buf = (char *)arg;
	i = 0;
	
	va_start(args, arg);
	while (buf[i] != NULL) {
		if (++i == 16) {
			return -1;
		}
		buf[i] = (char *)va_arg(args, const char *);
	}
	va_end(args);
	return execv(path, buf);
}
