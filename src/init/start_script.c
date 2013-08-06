/**
 * @file
 * @brief Runs start script
 *
 * @date 04.10.11
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>

#include <util/array.h>
#include <embox/unit.h>

#include <framework/cmd/api.h>

#include <cmd/shell.h>
#include <kernel/printk.h>

#define BUF_INP_SIZE OPTION_GET(NUMBER,input_buffer)
#define START_SHELL OPTION_GET(NUMBER,shell_start)

EMBOX_UNIT_INIT(run_script);

static const char *script_commands[] = {
	#include <start_script.inc>
};

#if START_SHELL
static int setup_tty(const char *dev_name) {
	int fd;
	char full_name[PATH_MAX];

	putenv("TERM=emterm");

	if (strlen(dev_name) == 0) {
		return -EINVAL;
	}

	strncpy(full_name, "/dev/", sizeof(full_name));
	strcat(full_name, dev_name);

	if (-1 == (fd = open(full_name, O_RDWR))) {
		return -errno;
	}

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	if (fd > 2) {
		close(fd);
	}

	return 0;
}
#endif

static int run_script(void) {
	const char *command;
	const struct shell *shell;

	shell = shell_lookup(OPTION_STRING_GET(shell_name));
	if (NULL == shell) {
		shell = shell_any();
		if (NULL == shell) {
			return -ENOENT;
		}
		setenv("shell", shell->name, 0);
	}

	printk("loading start script:\n");
	array_foreach(command, script_commands, ARRAY_SIZE(script_commands)) {
		printk("> %s \n", command);

		shell_exec(shell, command);
	}

#if START_SHELL
	setup_tty(OPTION_STRING_GET(tty_dev));

	printf("\nStarted shell [%s] on device [%s]\n",
		OPTION_STRING_GET(shell_name), OPTION_STRING_GET(tty_dev));

	shell_run(shell);
#endif

	return 0;
}
