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

static int exec(int argsc, char **argsv) {
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc - 1, argsv, "h");
		switch(nextOption) {
		case 'h':
			print_usage();
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	if (argsc > 1) {
		vfs_add_path(argsv[argsc - 1], NULL);
	}

	return 0;
}
