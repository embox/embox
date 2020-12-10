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
	int fd;
	int mode;

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
		int res;

		mode = O_WRONLY;

		if (0 == strcmp((const char *) argv[argc - 2], ">>")) {
			mode |= O_APPEND;
		} else if (0 == strcmp((const char *) argv[argc - 2], ">")) {
			mode |= O_CREAT | O_TRUNC;
		}
		else {
			print_usage();
			return 0;
		}

		fd = open((const char *) argv[argc - 1], mode,  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (fd < 0) {
			return -errno;
		}

		res = write(fd, (const void *) argv[1], strlen((const char *) argv[1]));
		if (0 > res) {
			close(fd);
			return -EIO;
		}
		write_items_count = res;
		res = write(fd, (const void *) "\n", 1);
		if (0 > res) {
			close(fd);
			return -EIO;
		}
		write_items_count += res;

		close(fd);
		return 0;
	}
	else if (argc == 2) {
		printf("%s \n",argv[argc - 1]);
	}
	else {
		print_usage();
	}
	return 0;
}
