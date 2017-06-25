/**
 * @file
 * @brief Create an empty file
 *
 * @date 24.04.2012
 * @author Andrey Gazukin
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

static void print_usage(void) {
	printf("Usage: touch full_filename ...\n");
}

int main(int argc, char **argv) {
	int opt;
	char *point;
	int fd;

	getopt_init();
	while (-1 != (opt = getopt(argc - 1, argv, "h"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			return 0;
		}
	}

	if (argc > 1) {
		point = argv[argc - 1];
		if (0 > (fd = creat(point, 0666))) {
			return -errno;
		}

		close(fd);
	}

	return 0;
}
