/*
 * @file
 * @brief Show all available command.
 *
 * @date 02.03.2009
 * @author Alexandr Batyukov
 */
#include <shell_command.h>

#define COMMAND_NAME     "help"
#define COMMAND_DESC_MSG "show all available command"
#define HELP_MSG         "Usage: help [-h]"

static const char *man_page =
	#include "help_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	int nextOption;
	SHELL_COMMAND_DESCRIPTOR * shell_desc;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "h");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	printf("Available commands: \n");
	for (shell_desc = shell_command_descriptor_find_first((char *) NULL, 0);
	    NULL != shell_desc;
	    shell_desc = shell_command_descriptor_find_next(shell_desc, (char *) NULL, 0)) {
		printf("%11s\t - %s\n", shell_desc->name, shell_desc->description);
	}
	return 0;
}
