/**
 * @file
 * @brief Show all available commands.
 *
 * @date 02.03.2009
 * @author Alexandr Batyukov
 */

#include <embox/cmd.h>

#include <getopt.h>
#include <stdio.h>
#include <cmd/framework.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: help [-h]\n");
}

// XXX remove. -- Eldar
#include <shell_command.h>
static int exec(int argc, char **argv) {
	const struct cmd *cmd;
	int opt;
	SHELL_COMMAND_DESCRIPTOR * shell_desc;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case '?':
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	printf("Available commands: \n");
	cmd_foreach(cmd) {
		printf("%11s - %s\n", cmd_name(cmd), cmd_brief(cmd));
	}

	// XXX remove. -- Eldar
	printf("DEPRECATED Available commands: \n");
	for (shell_desc = shell_command_descriptor_find_first((char *) NULL, 0); NULL
			!= shell_desc; shell_desc = shell_command_descriptor_find_next(
			shell_desc, (char *) NULL, 0)) {
		printf("%11s - %s\n", shell_desc->name, shell_desc->description);
	}

	return 0;
}
