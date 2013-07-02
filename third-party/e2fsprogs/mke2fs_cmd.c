/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    2.07.2013
 */

#include <embox/cmd.h>
#include <assert.h>
#include <unistd.h>

EMBOX_CMD(exec);

extern int main_mke2fs(int argc, char **argv);

static int exec(int argc, char **argv) {
	getopt_init();
	return main_mke2fs(argc, argv);
}
