/**
 * @file
 * @brief Runs start script
 *
 * @date 04.10.11
 * @author Alexander Kalmuk
 */

#include <util/array.h>
#include <embox/unit.h>
#include <ctype.h>
#include <framework/cmd/api.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <cmd/shell.h>
#include <unistd.h>
#include <fcntl.h>

#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <kernel/printk.h>

#define BUF_INP_SIZE OPTION_GET(NUMBER,input_buffer)

EMBOX_UNIT_INIT(run_script);

static const char *script_commands[] = {
	#include <start_script.inc>
};

static int run_cmd(int argc, char *argv[]) {
	const struct cmd *cmd;
	int code;

	if (argc == 0) {
		return 0;
	}

	if (NULL == (cmd = cmd_lookup(argv[0]))) {
		printk("%s: Command not found\n", argv[0]);
		return 0;
	}

	if (0 != (code = cmd_exec(cmd, argc, argv))) {
		printk("%s: Command returned with code %d: %s\n",
			cmd_name(cmd), code, strerror(-code));
	}
	return code;
}

static int parse(const char *const_line) {
	char *token_line[(BUF_INP_SIZE + 1) / 2];
	char cline[BUF_INP_SIZE];
	char *line = cline;
	int tok_pos = 0;
	int last_was_blank = 1;

	strncpy(cline, const_line, BUF_INP_SIZE);
	while (*line != '\0') {
		if (last_was_blank && !isspace(*line)) {
			token_line[tok_pos++] = line;
		}
		last_was_blank = isspace(*line);
		if (isspace(*line)) {
			*line = '\0';
		}
		line++;
	}
	return run_cmd(tok_pos, token_line);
}

static void setup_tty(const char *dev_name) {
	int fd;
	char full_name[MAX_LENGTH_PATH_NAME];

	if (strlen(dev_name) == 0) {
		return;
	}

	strncpy(full_name, "/dev/", sizeof(full_name));
	strcat(full_name, dev_name);

	if (-1 == (fd = open(full_name, O_RDWR))) {
		return;
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
}

static int run_script(void) {
	const char *command;
	const struct shell *shell;
	printk("\nStarting shell [%s] at device [%s]\n",
		OPTION_STRING_GET(shell_name), OPTION_STRING_GET(tty_dev));
	setup_tty(OPTION_STRING_GET(tty_dev));

	printk("loading start script:\n");
	array_foreach(command, script_commands, ARRAY_SIZE(script_commands)) {
		printk("> %s \n", command);
		parse(command);
	}

	shell = shell_lookup(OPTION_STRING_GET(shell_name));
	if (NULL == shell) {
		shell = shell_any();
		assert(shell);
	}

	shell->exec();

	return 0;
}
