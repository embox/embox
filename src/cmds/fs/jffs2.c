/**
 * @file
 * @brief Creates special file in virtual file systems
 *
 * @date 23.08.2013
 * @author Andrey Gazukin
 */

#include <embox/cmd.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <drivers/flash/flash.h>
#include <drivers/flash/flash_dev.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: jffs2 DEV NAME\n");
}

static int exec(int argc, char **argv) {
	int opt;


	getopt_init();
	while (-1 != (opt = getopt(argc - 1, argv, "h"))) {
		switch (opt) {
			case 'h': /* help message */
				print_usage();
				return 0;
			default:
				break;
		}
	}

	if (argc > 2) {
		return flash_emu_dev_init(argv);
	}

	return 0;
}
