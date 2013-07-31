/**
 * @file
 * @brief
 *
 * @date 30.07.2013
 * @author Andrey Gazukin
 */


#include <embox/cmd.h>
#include <assert.h>
#include <unistd.h>

EMBOX_CMD(exec);

extern int main_minigzip(int argc, char **argv);

static int exec(int argc, char **argv) {
	getopt_init();
	return main_minigzip(argc, argv);
}
