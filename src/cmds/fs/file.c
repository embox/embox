/*
 * @file
 * @brief Get file type by it's header
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-12-10
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <fs/file_format.h>

static void print_usage(void) {
	printf("Usage: file [FILE PATH]\n");
}

int main(int argc, char **argv) {
	FILE *fd;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	if (NULL == (fd = fopen(argv[argc - 1], "r"))) {
		printf("Can't open file %s\n", argv[argc - 1]);
		return 0;
	}

	switch (libc_get_file_format(fd)) {
	case TEXT_FILE:
		printf("%s: Text file\n", argv[argc - 1]);
		break;
	default:
		break;
	}

	return 0;
}
