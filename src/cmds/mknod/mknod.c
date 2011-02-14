/**
 * @file
 * @brief Creates special file in virtual file systems
 *
 * @date 15.10.2010
 * @author Anton BOndarev
 */
#include <types.h>
#include <shell_command.h>
#include <fs/vfs.h>

#define COMMAND_NAME     "mknod"
#define COMMAND_DESC_MSG "create special file"
#define HELP_MSG         "Usage: mknod NAME"

static const char *man_page =
#include "mknod_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	int nextOption;

	getopt_init();
	do {
		nextOption = getopt(argsc - 1, argsv, "h");
		switch (nextOption) {
			case 'h': /* help message */
				show_help();
				return 0;
			default:
				break;
		}
	} while (-1 != nextOption);

	if (argsc > 1) {
		vfs_add_path(argsv[argsc - 1], NULL);
	}
	return 0;
}
