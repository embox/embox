/**
 * @file
 * @brief simple Embox shell
 *
 * @date 13.11.2010
 * @author Fedor Burdun
 */

#include <embox/unit.h>
#include <kernel/printk.h>
#include <lib/readline.h>
#include <stdio.h>
#include <cmd/framework.h>
#include <cmd/cmdline.h>

EMBOX_UNIT(esh_start, esh_stop);

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

void esh_run(void) {
	char *cmdline;

	printf("\n%s\n", CONFIG_SHELL_WELCOME_MSG);

	for (;;) {
		cmdline = readline(CONFIG_SHELL_PROMPT);
		parse_cmdline(cmdline);
		freeline(cmdline);
		printf("\n");
	}
}

static int esh_start(void) {
	printk("ESH: ");
#ifndef CONFIG_TTY_CONSOLE_COUNT
	esh_run();
#else
	scheduler_start();
#endif
	printk(" [ done ]\n");
	return 0;
}

static int esh_stop(void) {
	return 0;
}

