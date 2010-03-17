/**
 * @file
 * @brief Concatenate files to standard output.
 *
 * @date 17.03.10
 * @author Nikolay Korotky
 */
#include <shell_command.h>
#include <stdio.h>
#include <string.h>

#define COMMAND_NAME     "cat"
#define COMMAND_DESC_MSG "concatenate files and print on the standard output"
#define HELP_MSG         "Usage: cat [FILES]"
static const char *man_page =
	#include "cat_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
	int nextOption;
	FILE *fd;
	char buff[4];
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

	if(argsc < 2) {
		show_help();
		return 0;
	}
	fd = fopen(argsv[argsc - 1], "r");
	while(fread(buff, sizeof(buff), 1, fd) > 0) {
		printf("%s", buff);
	}
	fclose(fd);
	return 0;
}
