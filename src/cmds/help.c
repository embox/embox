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

static int exec(int argc, char **argv) {
	const struct cmd *cmd;
	int opt;

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

	return 0;
}
