/*
 * shell_cmd.c
 *
 *  Created on: 1 avr. 2013
 *      Author: fsulima
 */


#include <embox/cmd.h>
#include <assert.h>

EMBOX_CMD(exec);

extern int smbclient_main(int argc, char **argv);

static int exec(int argc, char **argv) {
	return smbclient_main(argc, argv);
}
