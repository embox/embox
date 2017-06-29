/**
 * @file
 * @brief
 *
 * @date 25.04.2012
 * @author Andrey Gazukin
 */

#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


static void print_usage(void) {
	printf("Usage: echo \"STRING\" >> FILE\n");
}

int main(int argc, char **argv) {
	int opt;
	FILE *fd;
	const char *mode;

	while (-1 != (opt = getopt(argc - 1, argv, "h"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			break;
		}
	}

	if (argc > 3) {
		size_t write_items_count;

		if (0 == strcmp((const char *) argv[argc - 2], ">>")) {
			mode = "a";
		} else if (0 == strcmp((const char *) argv[argc - 2], ">")) {
			mode = "w";
		}
		else {
			print_usage();
			return 0;
		}

		if (NULL == (fd = fopen((const char *) argv[argc - 1], mode))) {
			return -errno;
		}

		write_items_count = fwrite((const void *) argv[1], strlen((const char *) argv[1]), 1, fd);
		write_items_count += fwrite((const void *) "\n", 1, 1, fd);
		fclose(fd);
		return write_items_count > 0 ? 0 : -EIO;
	}
	else if (argc == 2) {
		printf("%s \n",argv[argc - 1]);
	}
	else {
		print_usage();
	}
	return 0;
}
