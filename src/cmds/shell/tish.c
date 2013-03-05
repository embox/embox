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
#include <stdlib.h>
#include <ctype.h>
#include <kernel/task.h>
#include <lib/linenoise.h>
#include <cmd/cmdline.h>
#include <embox/unit.h>

#include <framework/cmd/api.h>

#include <cmd/shell.h>

#define BUF_INP_SIZE OPTION_GET(NUMBER, prompt_len)

struct cmdtask_data {
	int argc;
	char buf[BUF_INP_SIZE];
};

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

	if (NULL == (cmd = cmd_lookup(argv[0]))) {
		printf("%s: Command not found\n", argv[0]);
		return -ENOENT;
	}

	if (0 != (code = cmd_exec(cmd, argc, argv))) {
		printf("%s: Command returned with code %d: %s\n",
				cmd_name(cmd), code, strerror(-code));
	}
	return code;
}

static void *cmdtask(void *data) {
	struct cmdtask_data *m = (struct cmdtask_data *) data;
	char *argv[(BUF_INP_SIZE + 1) / 2], **pp, *p;

	pp = argv;
	p = m->buf;

	while (*p != '\0') {
		*pp++ = p;
		p += strlen(p) + 1;
	}

	run_cmd(m->argc, argv);

	free(m);

	return NULL;

}

static int process(int argc, char *argv[]) {
	if (argc == 0) {
		return 0;
	}

	if (!strcmp(argv[argc - 1], "&")) {
		struct cmdtask_data *m = malloc(sizeof(struct cmdtask_data));
		char *p = m->buf;

		if (!m) {
			return -ENOMEM;
		}

		m->argc = argc - 1;
		for (int i = 0; i < argc - 1; i++) {
			strcpy(p, argv[i]);
			p += strlen(p) + 1;
		}

		*p = '\0';

		return new_task(argv[0], cmdtask, m);
	}

	return run_cmd(argc, argv);
}

int shell_line_input(const char *cmdline) {
	char cmdl[BUF_INP_SIZE];
	/* In the worst case cmdline looks like "x x x x x x". */
	char *argv[(BUF_INP_SIZE + 1) / 2];
	int argc;

	if (strlen(cmdline) >= BUF_INP_SIZE) {
		return -ERANGE;
	}

	strcpy(cmdl, cmdline);

	if (0 == (argc = cmdline_tokenize(cmdl, argv))) {
		return 0;
	}

	return process(argc, argv);
}

static void tish_run(void) {
	const char *prompt = OPTION_STRING_GET(prompt);
	char inp_buf[BUF_INP_SIZE];
	struct hist h;

	linenoise_history_init(&h);

	printf("\n%s\n\n", OPTION_STRING_GET(welcome_msg));

	while (1) {
		if (linenoise(prompt, inp_buf, BUF_INP_SIZE, &h,
			(compl_callback_t) cmd_compl) < 0) {
			return;
		}

		inp_buf[strlen(inp_buf) - 1] = '\0';
		linenoise_history_add(inp_buf, &h);
		shell_line_input(inp_buf);
	}
}

SHELL_DEF({
	.name = "tish",
	.exec = shell_line_input,
	.run  = tish_run,
	});

