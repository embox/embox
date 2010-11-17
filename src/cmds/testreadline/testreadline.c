/**
 * @file TODO delete after terminal debugging
 *
 * @brief Temporary cmd for terminal debugging
 *
 * @date 13.11.2010
 * @author Anton Bondarev
 */

#include <shell_command.h>
#include <lib/readline.h>

#define COMMAND_NAME     "testreadline"
#define COMMAND_DESC_MSG "displays info associated with compilation"
#define HELP_MSG         "Usage: version [-h]"

static const char *man_page = "temporary command";

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	char *line;

	FILE *ff = fopen("/dev/uart","r");

	line = readline(CONFIG_SHELL_PROMPT);
	TRACE("\nreceived '%s'", line);
	freeline(line);

	fclose(ff);
	return 0;
}
