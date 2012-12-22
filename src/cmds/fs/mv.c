/**
 * @file
 * @brief Move or rename file.
 *
 * @date 18.12.12
 * @author Ivan Tretyakov
 */


#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <fs/sys/fsop.h>

#include <embox/cmd.h>

#define CMD	"mv"

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: "CMD" [-rfh] path\n"
			"\t-r - recursive\n"
			"\t-f - do not prompt before overwrite\n"
			"\t-h - print this help\n"
			"Options don't implemented yet\n");
}

static int exec(int argc, char **argv) {
	int opt;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "rfh"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return ENOERR;
		case 'r':
			break;
		case 'f':
			break;
		default:
			printf(CMD": invalid option -- '%c'\n", optopt);
			return EINVAL;
		}
	}

	/* At the moment command support only two files as arguments.
	 * In future it would be able to copy many source paths into the same
	 * destination path */
	if (2 == (argc - optind)) {
		return rename(argv[argc - 2], argv[argc - 1]);
	}

	print_usage();

	return ENOERR;
}
