/**
 * @file
 * @brief Creates special file in virtual file systems
 *
 * @date 15.10.10
 * @author Anton BOndarev
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

	getopt_init();
	do {
		opt = getopt(argc - 1, argv, "h");
		switch (opt) {
			case 'h': /* help message */
				print_usage();
				return 0;
			default:
				break;
		}
	} while (-1 != opt);

	if (argc > 1) {
		vfs_add_path(argv[argc - 1], NULL);
	}
	return 0;
}
