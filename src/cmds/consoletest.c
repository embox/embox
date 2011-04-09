/**
 * @file
 * @brief Print information about current console.
 *
 * @date 02.04.09
 * @author Fedor Burdun
 */

//#include <drivers/vconsole.h>
extern void* cur_console;

#include <stdio.h>
#include <embox/cmd.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	if (argc<2) return 0;

	char str[100];
	strcpy(&str[0],argv[1]);

	while (true) {
		printf("%s",&str[0]);
	}

	return 0;
}
