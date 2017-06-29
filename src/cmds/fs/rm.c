/**
 * @file
 * @brief rm file
 *
 * @date 22.08.09
 * @author Roman Evstifeev
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

static void print_usage(void) {
	printf("Usage: rm [OPTIONS] FILE\n");
}

int main(int argc, char **argv) {
	const char *file_path;
	int opt;
	getopt_init();
	while (-1 != (opt = getopt(argc - 1, argv, "frh"))) {
		switch(opt) {
		case 'f':
			break;
		case 'r':
			break;
		case 'h':
			print_usage();
			return 0;
		default:
			return -EINVAL;
		}
	}

	file_path = argv[argc - 1];

	return remove(file_path);
}
