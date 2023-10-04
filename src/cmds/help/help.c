/**
 * @file
 * @brief Show all available commands.
 *
 * @date 02.03.09
 * @author Alexandr Batyukov
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <framework/cmd/api.h>

static void print_usage(void) {
	printf("Usage: help [-h]\n");
}

int main(int argc, char **argv) {
	const struct cmd *cmd;
	int opt, tab = 10;
	char fmt[20];

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

	cmd_foreach(cmd) {
		if (strlen(cmd_name(cmd)) > tab) {
			tab = strlen(cmd_name(cmd));
		}
	}

	snprintf(fmt, sizeof(fmt), "%%%ds - %%s\n", tab + 1);

	printf("Available commands: \n");
	cmd_foreach(cmd) {
		printf(fmt, cmd_name(cmd), cmd_brief(cmd));
	}

	return 0;
}
