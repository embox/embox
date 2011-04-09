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
	printf("Current console ptr: 0x%lx\n",cur_console);
	printf("----------------------------------------------------------------------------------------------------\n");
	//printf("Current console id: %ld\n",cur_console->id);
	return 0;
}
