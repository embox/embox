/**
 * @file
 * @brief display the on-line manual pages
 *
 * @date 23.09.09
 * @author Nikolay Korotky
 */
#include <shell_command.h>
#include <string.h>
#include <stdio.h>

#include <cmd/framework.h>

#define COMMAND_NAME     "man"
#define COMMAND_DESC_MSG "display the on-line manual pages"
#define HELP_MSG         "Usage: man name"
static const char *man_page =
#include "man_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page)
;

static int exec(int argsc, char **argsv) {
	const struct cmd *cmd;
	char *name;
	SHELL_COMMAND_DESCRIPTOR *shell_desc;

	if (argsc > 1) {
		name = argsv[argsc - 1];
	} else {
		show_help();
		return -1;
	}

	if (NULL == (cmd = cmd_lookup(name))) {
		shell_desc = shell_command_descriptor_find_first(name, strlen(name));
		if (shell_desc != NULL) {
			printf("%s: Man for command using deprecated API.\n\n", name);
			printf("%s\n", *shell_desc->man_page);
		} else {
			printf("No manual entry for %s\n", name);
		}
		return 0;
	}

	printf("%s\n", cmd_details(cmd));

}
