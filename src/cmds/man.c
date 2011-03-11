/**
 * @file
 * @brief display the on-line manual pages
 *
 * @date 23.09.09
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include <util/array.h>
#include <cmd/framework.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: man cmd ...\n");
}

// XXX remove. -- Eldar
#include <shell_command.h>

static int exec(int argc, char **argv) {
	const struct cmd *cmd;
	char *name;
	SHELL_COMMAND_DESCRIPTOR *shell_desc;

	if (argc <= 1) {
		print_usage();
		return -1;
	}

	array_foreach(name, argv + 1, argc - 1) {
		if (NULL != (cmd = cmd_lookup(name))) {
			printf("Manual entry for %s:\n%s\n\n", name, cmd_details(cmd));
		} else {
			// XXX remove. -- Eldar
			shell_desc = shell_command_descriptor_find_first(name, strlen(name));
			if (shell_desc != NULL) {
				printf("%s: Man for command using deprecated API.\n", name);
				printf("%s\n\n", *shell_desc->man_page);
			} else {
				printf("No manual entry for %s\n\n", name);
			}
		}
	}

	return 0;
}
