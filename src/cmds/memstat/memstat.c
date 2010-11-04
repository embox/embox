/**
 * @file
 * @brief print memory statistics
 *
 * @date 5.11.10
 * @author Gleb Efimov
 */
#include <shell_command.h>

#define COMMAND_NAME     "memstat"
#define COMMAND_DESC_MSG "print memory statistics"
#define HELP_MSG         "Usage: memstat [-h]"

static const char *man_page =
	#include "memstat_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	TRACE(" Command is not implemented =) \n ");

	return 0;
}
