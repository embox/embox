 /**
 * \file cp.c
 *
 * \date 22.08.2009
 * \author zoomer
 */

#include "common.h"
#include "conio.h"
#include "shell.h"
#include "shell_command.h"

#include "file_new.h"
/*
static char cp_keys[] = {
#include "ldimg_keys.inc"
		};
*/
static void show_help() {
	printf("help\n");
}

#define COMMAND_NAME "rm"
#define COMMAND_DESC_MSG "rm file"
static const char *help_msg = "rm";
#define HELP_MSG help_msg

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG);


static int exec(int argsc, char **argsv)
{
	const char *file_path;

	if (argsc < 1) {
		show_help();
		return -1;
	}

	file_path = argsv[0];

	if (!remove(file_path))
		return -1;

	return 0;
}
