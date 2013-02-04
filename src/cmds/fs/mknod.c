/**
 * @file
 * @brief Creates special file in virtual file systems
 *
 * @date 15.10.10
 * @author Anton Bondarev
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <types.h>
#include <fs/vfs.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: mknod NAME\n");
}

static int exec(int argc, char **argv) {
	int opt;
	mode_t mode;

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

	mode = S_IFBLK; // XXX this should me an argument, creating block dev
	mode |= S_IRALL | S_IWALL; // TODO umask. -- Eldar

	if (argc > 1) {
		vfs_create(NULL, argv[argc - 1], mode);
	}

	return 0;
}
