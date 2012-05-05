/**
 * @file
 * @brief Create an empty file
 *
 * @date 24.04.2012
 * @author Andrey Gazukin
 */
#include <embox/cmd.h>
#include <fcntl.h>
#include <getopt.h>
#include <fs/vfs.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: touch full_filename ...\n");
}

static int exec(int argc, char **argv) {
	int opt;
	getopt_init();
	while (-1 != (opt = getopt(argc - 1, argv, "h"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			return 0;
		}
	}

	if (argc > 1) {
		creat(argv[argc - 1], 0);
	}

	return 0;
}



