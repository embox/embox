/**
 * @file
 * @brief make directories
 *
 * @date 15.10.10
 * @author Nikolay Korotky
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <embox/cmd.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: mkdir [ -m MODE ] DIR ...\n");
}

static int exec(int argc, char **argv) {
	int opt;
	char *point;
	int mode = 0;

	getopt_init();
	while (-1 != (opt = getopt(argc - 1, argv, "hm:"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		case 'm':
			mode = strtol(optarg, NULL, 8);
			break;
		default:
			return 0;
		}
	}

	if (argc > 1) {
		point = argv[argc - 1];
		if (-1 == mkdir(point, mode)) {
			return -errno;
		}
	}

	return 0;
}
