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
#include <posix/stdio.h>
#include <errno.h>
#include <string.h>

#define BUF_INP_SIZE 15

EMBOX_UNIT_INIT(run);

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
		printf("%s: Command not found\n", argv[0]);
		return 0;
	}

	if (0 != (code = cmd_exec(cmd, argc, argv))) {
		printf("%s: Command returned with code %d: %s\n", cmd_name(cmd), code,
				strerror(-code));
	}
	return code;
}

static int parse(char *line) {
	char *token_line[(BUF_INP_SIZE + 1) / 2];int tok_pos = 0;
	int last_was_blank = 1;
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

static int run(void) {
	const char *command;
	array_foreach(command, script_commands, ARRAY_SIZE(script_commands)) {
		parse((char *) command);
	}

	return 0;
}
