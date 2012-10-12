 /**
 * @file
 * @brief rm file
 *
 * @date 22.08.09
 * @author Roman Evstifeev
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: rm [OPTIONS] FILE\n");
}

static int exec(int argc, char **argv) {
	const char *file_path;
	//int recursive, ignore;
	int opt;
	getopt_init();
	while (-1 != (opt = getopt(argc - 1, argv, "frh"))) {
		switch(opt) {
		case 'f':
          //              ignore = 1;
			break;
		case 'r':
	//		recursive = 1;
			break;
		case 'h':
			print_usage();
			return 0;
		default:
			return -1;
		}
	}

	file_path = argv[argc - 1];
	//TODO:

	return remove(file_path);
}
