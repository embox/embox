/**
 * @file
 * @brief <BRIEF>
 *
 * @date <DATE>
 * @author <AUTHOR>
 */
#include <shell_command.h>

#define COMMAND_NAME     "cmd_template"
#define COMMAND_DESC_MSG "cmd description message"
#define HELP_MSG         "Usage: cmd_template ..."

static const char *man_page =
	#include "cmd_template_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
        /* use getopt() */
	show_help();

	return 0;
}
