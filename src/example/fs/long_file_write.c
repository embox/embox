/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.02.2014
 */

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <framework/example/self.h>

EMBOX_EXAMPLE(long_file_write);

static int long_file_write(int argc, char **argv) {
	int fd;
	int i;

	if (argc != 2) {
		fprintf(stderr, "Run this with filename arg\n");
		return 0;
	}

	if (-1 == (fd = open(argv[1], O_WRONLY | O_TRUNC))) {
		return -errno;
	}

	for (i = 0; i < 500000; i++) {
		write(fd, "a", 1);
	}

	return 0;
}
