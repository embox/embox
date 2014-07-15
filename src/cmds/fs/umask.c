/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    27.06.2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <embox/cmd.h>

EMBOX_CMD(umask_main);

static int umask_main(int argc, char *argv[]) {

	if (argc != 1) {
		mode_t umode;
		umode = strtol(argv[2], NULL, 8);
		umask(umode);
	} else {
		mode_t umode;
		umode = umask(0);
		printf("%04o\n", umode);
		umask(umode);
	}

	return 0;
}

