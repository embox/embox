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

#include <prom/prom_printf.h>

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
		prom_printf("%s: Command not found\n", argv[0]);
		return 0;
	}

	if (0 != (code = cmd_exec(cmd, argc, argv))) {
		prom_printf("%s: Command returned with code %d: %s\n", cmd_name(cmd), code,
				strerror(-code));
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


static int run_script(void) {
	const char *command;
	const struct shell *shell;

	prom_printf("\nloading start script:\n");
	array_foreach(command, script_commands, ARRAY_SIZE(script_commands)) {
		prom_printf("> %s \n", command);
		parse(command);
	}

	shell = shell_lookup(OPTION_STRING_GET(shell_name));
	assert(shell);

	shell->exec();

	return 0;
}


