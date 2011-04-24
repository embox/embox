/**
 * @file
 * @brief make directories
 *
 * @date 15.10.10
 * @author Nikolay Korotky
 */
#include <embox/cmd.h>
#include <getopt.h>
#include <fs/vfs.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: mkdir DIR ...\n");
}

static int exec(int argc, char **argv) {
	int opt;
	getopt_init();
	do {
		opt = getopt(argc - 1, argv, "h");
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != opt);

	if (argc > 1) {
		vfs_add_path(argv[argc - 1], NULL);
	}

	return 0;
}
