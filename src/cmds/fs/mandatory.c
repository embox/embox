/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.02.2013
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <utmp.h>

#include <embox/cmd.h>

EMBOX_CMD(mandatory);

static int mandatory(int argc, char **argv) {
	printf(".....  Done\n");

	return 0;
}
