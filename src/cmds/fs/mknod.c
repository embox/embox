/**
 * @file
 * @brief Creates special file in virtual file systems
 *
 * @date 15.10.10
 * @author Anton Bondarev
 */

#include <unistd.h>
#include <stdio.h>
#include <fs/vfs.h>

static void print_usage(void) {
	printf("Usage: mknod NAME\n");
}

int main(int argc, char **argv) {
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

	mode = S_IFBLK; // XXX this should be an argument, creating block dev
	mode |= S_IRALL | S_IWALL; // TODO umask. -- Eldar

	if (argc > 1) {
		struct path root, child;
		vfs_get_root_path(&root);
		vfs_create(&root, argv[argc - 1], mode, &child);
	}

	return 0;
}
