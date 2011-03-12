 /**
 * @file
 * @brief rm file
 *
 * @date 22.08.2009
 * @author Roman Evstifeev
 */
#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: rm [OPTIONS] FILE\n");
}

static int exec(int argsc, char **argsv) {
	const char *file_path;
	int recursive, ignore;
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc - 1, argsv, "frh");
		switch(nextOption) {
		case 'f':
                        ignore = 1;
			break;
		case 'r':
			recursive = 1;
			break;
		case 'h':
			print_usage();
			return 0;
		case -1:
			break;
		default:
			return -1;
		}
	} while (nextOption != -1);

	file_path = argsv[argsc - 1];
	//TODO:

	return remove(file_path);
}
