/**
 * @file
 * @brief rm file
 *
 * @date 22.08.09
 * @author Roman Evstifeev
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void print_usage(void) {
	printf("Usage: rm [OPTIONS] FILE\n");
}

int main(int argc, char **argv) {
	const char *file_path;
	int opt;

	while (-1 != (opt = getopt(argc - 1, argv, "frh"))) {
		switch (opt) {
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

	if (-1 == remove(file_path)) {
		return -errno;
	}

	return 0;
}
