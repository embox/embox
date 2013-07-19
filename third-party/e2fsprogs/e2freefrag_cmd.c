/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    30.06.2013
 */

#include <embox/cmd.h>
#include <assert.h>
#include <unistd.h>

EMBOX_CMD(exec);

extern int main_e2freefrag(int argc, char **argv);

static int exec(int argc, char **argv) {
	getopt_init();
	return main_e2freefrag(argc, argv);
}
