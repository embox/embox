/**
 * \file man.c
 * \date 23.09.09
 * \author Sikmir
 */
#include "shell_command.h"

#define COMMAND_NAME     "man"
#define COMMAND_DESC_MSG "display the on-line manual pages"
static const char *help_msg =
	#include "man_help.inc"
	;
#define HELP_MSG help_msg

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG);

static int exec(int argsc, char **argsv) {
	//TODO:
	return 0;
}
