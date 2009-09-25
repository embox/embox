/**
 * \file man.c
 * \date 23.09.09
 * \author Sikmir
 */
#include "shell_command.h"
#include "string.h"

#define COMMAND_NAME     "man"
#define COMMAND_DESC_MSG "display the on-line manual pages"
#define HELP_MSG         "Usage: man name"
static const char *man_page =
	#include "man_help.inc"
	;

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	char *name = argsv[argsc - 1];
	SHELL_COMMAND_DESCRIPTOR *shell_desc;
	shell_desc = shell_command_descriptor_find_first(name, strlen(name));
	TRACE("%s\n", *shell_desc->man_page);
	return 0;
}
