/**
 * @file
 * @brief make directories
 *
 * @date 15.10.10
 * @author Nikolay Korotky
 */
#include <shell_command.h>
#include <fs/vfs.h>

#define COMMAND_NAME     "mkdir"
#define COMMAND_DESC_MSG "make directories"
#define HELP_MSG         "Usage: mkdir DIR ..."

static const char *man_page =
	#include "mkdir_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	const char *path;

	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc - 1, argsv, "h");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while(-1 != nextOption);

	if(argsc > 1) {
		path = argsv[argsc - 1];
	}

	vfs_add_path(path, NULL);
	return 0;
}
