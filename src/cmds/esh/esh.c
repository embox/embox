/**
 * @file
 * @brief simple Embox shell
 *
 * @date 13.11.10
 * @author Fedor Burdun
 */

/*
 * Now shell works in canonical mode.
 */

#include <embox/cmd.h>

#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <kernel/printk.h>
#include <lib/readline.h>
#include <cmd/cmdline.h>
#include <framework/cmd/api.h>

EMBOX_CMD(exec);

#define CMDLINE_MAX_LENGTH 127

static void parse_cmdline(char *cmdline) {
	const struct cmd *cmd;
	int code;
	/* In the worst case cmdline looks like "x x x x x x". */
	char *argv[(CMDLINE_MAX_LENGTH + 1) / 2];
	int argc = 0;

	if (0 == (argc = cmdline_tokenize(cmdline, argv))) {
		/* Only spaces were entered */
		return;
	}

	if (NULL == (cmd = cmd_lookup(argv[0]))) {
		printf("%s: Command not found\n", argv[0]);
		return;
	}

	if (0 != (code = cmd_exec(cmd, argc, argv))) {
		printf("%s: Command returned with code %d: %s\n", cmd_name(cmd), code,
				strerror(-code));
	}
}

/* FIXME: GIVE ME MORE FUNCTIONS!!! */
static void esh_run(void) {
	char *cmdline;

	printf("\n%s\n", CONFIG_SHELL_WELCOME_MSG);

	for (;;) {
		cmdline = readline(CONFIG_SHELL_PROMPT);
		parse_cmdline(cmdline);
		freeline(cmdline);
		printf("\n");
	}
}


static int exec(int argc, char **argv) {
	esh_run();
	return 0;
}
