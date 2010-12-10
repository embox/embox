 /**
 * @file
 * @brief rm file
 *
 * @date 22.08.2009
 * @author Roman Evstifeev
 */
#include <shell_command.h>
#include <stdio.h>

#define COMMAND_NAME     "rm"
#define COMMAND_DESC_MSG "remove file or directory"
#define HELP_MSG         "Usage: rm [OPTIONS] FILE"

static const char *man_page =
	#include "rm_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	const char *file_path;
	int recursive, ignore;
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc - 1, argsv, "frh");
		switch(nextOption) {
		case 'f':
                        ignore = 1;
			break;
		case 'r':
			recursive = 1;
			break;
		case 'h':
			show_help();
			return 0;
		case -1:
			break;
		default:
			return -1;
		}
	} while (nextOption != -1);

	file_path = argsv[argsc - 1];
	//TODO:

	return remove(file_path);
}
