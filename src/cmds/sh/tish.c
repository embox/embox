/**
 * @file
 * @brief Tiny Shell
 * @details New shell build around tiny readline impl called linenoise.
 *	    Supports history and completions and tends to be extremely small.
 *
 * @date 13.09.11
 * @author Anton Kozlov
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <lib/linenoise.h>
#include <ctype.h>

#include <framework/cmd/api.h>

#include <cmd/shell.h>

#define BUF_INP_SIZE 128

static int cmd_compl(char *buf, char *out_buf) {
	const struct cmd *cmd = NULL;
	int buf_len = strlen(buf);
	int ret = 0;

	cmd_foreach(cmd) {
		if (strlen(cmd_name(cmd)) < buf_len) {
			continue;
		}
		if (strncmp(buf, cmd_name(cmd), buf_len) == 0) {
			strcpy(out_buf, cmd_name(cmd));
			out_buf += strlen(cmd_name(cmd)) + 1;
			ret++;
		}
	}
	return ret;

}

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

int shell_line_input(char *line) {
	char *token_line[(BUF_INP_SIZE + 1) / 2];
	int tok_pos = 0;
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

void shell_run(void) {
	const char *prompt = CONFIG_SHELL_PROMPT;
	char inp_buf[BUF_INP_SIZE];
	struct hist h;

	linenoise_history_init(&h);

	printf("\n%s", CONFIG_SHELL_WELCOME_MSG);

	while (1) {
		linenoise(prompt, inp_buf, BUF_INP_SIZE, &h, (compl_callback_t) cmd_compl);
		inp_buf[strlen(inp_buf) - 1] = '\0';
		linenoise_history_add(inp_buf, &h);
		shell_line_input(inp_buf);
	}
}
