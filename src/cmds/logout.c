/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.06.2014
 */

#include <stdlib.h>
#include <embox/cmd.h>

EMBOX_CMD(logout_main);

static int logout_main(int argc, char *argv[]) {
	exit(0);
}

