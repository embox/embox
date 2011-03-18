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

static int exec(int argsc, char **argsv) {
	int nextOption;

	getopt_init();
	do {
		nextOption = getopt(argsc - 1, argsv, "h");
		switch (nextOption) {
			case 'h': /* help message */
				print_usage();
				return 0;
			default:
				break;
		}
	} while (-1 != nextOption);

	if (argsc > 1) {
		vfs_add_path(argsv[argsc - 1], NULL);
	}
	return 0;
}
