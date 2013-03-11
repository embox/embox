/**
 * @file
 * @brief rm file
 *
 * @date 22.08.09
 * @author Roman Evstifeev
 */

#include <embox/cmd.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

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
//      ignore = 1;
			break;
		case 'r':
//		recursive = 1;
			break;
		case 'h':
			print_usage();
			return 0;
		default:
			return -EINVAL;
		}
	}

	file_path = argv[argc - 1];
	//TODO:

	/*TODO relative path*/
	if(0 != strncmp(file_path, "/", 1)) {
		return -EINVAL;
	}

	return remove(file_path);
}
