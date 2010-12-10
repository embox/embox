/**
 * @file
 * @brief display the on-line manual pages
 *
 * @date 23.09.09
 * @author Nikolay Korotky
 */
#include <shell_command.h>
#include <string.h>

#define COMMAND_NAME     "man"
#define COMMAND_DESC_MSG "display the on-line manual pages"
#define HELP_MSG         "Usage: man name"
static const char *man_page =
	#include "man_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	if (argsc > 1) {
		char *name = argsv[argsc - 1];
		SHELL_COMMAND_DESCRIPTOR *shell_desc;
		shell_desc = shell_command_descriptor_find_first(name, strlen(name));
		if (shell_desc != NULL) {
			TRACE("%s\n", *shell_desc->man_page);
		} else {
			TRACE("No manual entry for %s\n", name);
		}
	} else {
		show_help();
	}
	return 0;
}
