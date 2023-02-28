/**
 * @file
 *
 * @author Anton Bondarev
 * @date 27.02.2023
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


static void print_help(void) {
	printf("Usage: echo_and_wait <echo string> <tty device name>\n");
}

static void wait_gets(int fd) {
	uint8_t c[64] = {0};


	read(fd, &c, 64);
		printf("%s", c);

}

int main(int argc, char **argv) {
	int fd;
	int res;

	if (argc < 3) {
		print_help();
		return 0;
	}

	fd = open((const char *) argv[argc - 1], O_RDWR,  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0) {
		return -errno;
	}

	res = write(fd, (const void *) argv[1], strlen((const char *) argv[1]));
	if (0 > res) {
		close(fd);
		return -EIO;
	}

	res = write(fd, (const void *) "\n", 1);
	if (0 > res) {
		close(fd);
		return -EIO;
	}

	wait_gets(fd);

	close(fd);

	return 0;
}
