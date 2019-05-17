/**
 * @file
 * @brief Compute and check MD5 message digest.
 *
 * @note IMPORTANT NOTE This cmd works only in filesystems
 * which uses ram addresses to access files
 *
 * @date 10.03.10
 * @author Nikolay Korotky
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <errno.h>

#include <lib/crypt/md5.h>

static void print_usage(void) {
	printf("Usage: md5sum [FILE]\n");
}

int main(int argc, char **argv) {
	int opt;
	int fd;
	char buff[16];
	size_t size;
	struct stat st;
	md5_state_t state;
	md5_byte_t digest[16];
	int di;
	char hex_output[16 * 2 + 1];

	if (argc < 2) {
		print_usage();
		return -EINVAL;
	}

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case '?':
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	fd = open(argv[argc - 1], O_RDONLY);
	if (fd < 0) {
		printf("Can't open file %s\n", argv[argc - 1]);
		return errno;
	}

	if (0 != fstat(fd, &st)) {
		close(fd);
		return errno;
	}

	/* Compute MD5 sum */
	md5_init(&state);
	{
		while (0 < (size = read(fd, buff, sizeof(buff)))) {
			md5_append(&state, (const md5_byte_t *) buff, size);
		}
	}
	md5_finish(&state, digest);

	/* Prepare output */
	for (di = 0; di < 16; ++di) {
		sprintf(hex_output + di * 2, "%02x", digest[di]);
	}
	printf("%s %s\n", hex_output, argv[argc - 1]);

	close(fd);

	return 0;
}
