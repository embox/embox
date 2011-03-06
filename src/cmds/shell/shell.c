/**
 * @file
 *
 * @date 02.02.2009
 * @author Alexey Fomin
 */

#include <embox/unit.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <util/array.h>
#include <cmd/framework.h>

#include <shell_utils.h>
#include "console/console.h"

// XXX just for now -- Eldar
EMBOX_UNIT(shell_start, shell_stop);

#include <shell_command.h>

static void deprecated_exec_callback(int argc, char **argv) {
	SHELL_COMMAND_DESCRIPTOR *c_desc;

	if (NULL == (c_desc = shell_command_descriptor_find_first(argv[0], -1))) {
		return;
	}
	if (NULL == c_desc->exec) {
		return;
	}

	printf("%s: Executing command using deprecated API.\n\n", argv[0]);
	shell_command_exec(c_desc, argc, argv);
}

static void exec_callback(CONSOLE_CALLBACK *cb, CONSOLE *console, char *cmdline) {
	const struct cmd *cmd;
	int code;
	/* In the worst case cmdline looks like "x x x x x x". */
	char *argv[(CMDLINE_MAX_LENGTH + 1) / 2];
	int argc = 0;

	if (0 == (argc = parse_str(cmdline, argv))) {
		/* Only spaces were entered */
		return;
	}

	if (NULL == (cmd = cmd_lookup(argv[0]))) {
#if 0
		printf("%s: Command not found\n", argv[0]);
#else
		deprecated_exec_callback(argc, argv);
#endif
		return;
	}

	if (0 != (code = cmd_exec(cmd, argc, argv))) {
		printf("%s: Command returned with code %d: %s\n", cmd_name(cmd), code,
				strerror(-code));
	}
}

/**
 * Guesses command using its beginning
 *
 * -- Eldar
 */
static void guess_callback(CONSOLE_CALLBACK *cb, CONSOLE *console,
		const char* line, const int max_proposals, int *proposals_len,
		const char *proposals[], int *offset, int *common) {
	const struct cmd *cmd;
	int cursor = strlen(line);
	int start = cursor, i;
	char ch;
	// XXX
	SHELL_COMMAND_DESCRIPTOR * shell_desc;

	while (start > 0 && isalpha(line[start - 1])) {
		start--;
	}
	line += start;

	*offset = cursor - start;
	*proposals_len = 0;

	cmd_foreach(cmd) {
		if (0 == strncmp(cmd_name(cmd), line, *offset)) {
			proposals[(*proposals_len)++] = cmd_name(cmd);
		}
	}
	// XXX deprecated. -- Eldar
	for (shell_desc
			= shell_command_descriptor_find_first((char*) line, *offset); NULL
			!= shell_desc; shell_desc = shell_command_descriptor_find_next(
			shell_desc, (char *) line, *offset)) {
		proposals[(*proposals_len)++] = (char *) shell_desc->name;
	}

	*common = 0;
	if (*proposals_len == 0) {
		return;
	}
	while (1) {
		if ((ch = proposals[0][*offset + *common]) == '\0') {
			return;
		}
		for (i = 1; i < *proposals_len; ++i) {
			if (ch != proposals[i][*offset + *common]) {
				return;
			}
		}
		(*common)++;
	}
}

__extension__ static const char *script_commands[] = {
#include <start_script.inc>
		};

static void shell_start_script(CONSOLE *console, CONSOLE_CALLBACK *callback) {
	char buf[CMDLINE_MAX_LENGTH + 1];
	const char *command;

	array_static_foreach(command, script_commands) {
		strncpy(buf, command, sizeof(buf));
		printf("> %s \n", buf);
		exec_callback(callback, console, buf);
	}
}

static CONSOLE console[1];

static int shell_start(void) {
	static const char* prompt = CONFIG_SHELL_PROMPT;
	static CONSOLE_CALLBACK callback[1];

	callback->exec = exec_callback;
	callback->guess = guess_callback;
	if (console_init(console, callback) == NULL) {
		LOG_ERROR("Failed to create a console");
		return -1;
	}
	if (ARRAY_SIZE(script_commands)) {
		printf("\nStarting script...\n");
		shell_start_script(console, callback);
	}

	printf("\n%s", CONFIG_SHELL_WELCOME_MSG);
	console_start(console, prompt);
	return 0;
}

static int shell_stop(void) {
	console_stop(console);
	return 0;
}
