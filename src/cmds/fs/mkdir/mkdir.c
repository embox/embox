/**
 * @file
 * @brief make directories
 *
 * @date 15.10.10
 * @author Nikolay Korotky
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

static void print_usage(void) {
	printf("Usage: mkdir [-s] [-m MODE] DIR ...\n");
}

int main(int argc, char **argv) {
	int opt;
	char *point;
	int mode_set = 0;
	int mode = 0777;

	if (argc < 2) {
		printf("%s: missing operand\n", argv[0]);
		print_usage();
		return 0;
	}

	while (-1 != (opt = getopt(argc - 1, argv, "hm:v"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		case 'm':
			mode = strtol(optarg, NULL, 8);
			mode_set = 1;
			break;
		case 's':
		case 'v':
			mode |= VFS_DIR_VIRTUAL;
			break;
		default:
			return 0;
		}
	}

	if (argc > 1) {
		mode_t umode;
		point = argv[argc - 1];

		if (mode_set) {
			umode = umask(0);
		}

		if (-1 == mkdir(point, mode)) {
			return -errno;
		}

		if (mode_set) {
			umask(umode);
		}

	}

	return 0;
}
