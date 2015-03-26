/**
 * @file
 * @brief Compute and check MD5 message digest.
 *
 * @date 10.03.10
 * @author Nikolay Korotky
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>

#include <lib/crypt/md5.h>

#include <fs/file_operation.h>


static void print_usage(void) {
	printf("Usage: md5sum [FILE]\n");
}

int main(int argc, char **argv) {
	int opt;
	FILE *fd;
	stat_t st;
	uint32_t addr;
	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;

	getopt_init();
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

	/* Get size and file's base addr */
	if (NULL == (fd = fopen(argv[argc - 1], "r"))) {
		printf("Can't open file %s\n", argv[argc - 1]);
		return -errno;
	}
	fioctl(fd, 0, &addr);
	fclose(fd);
	stat((char *) argv[argc - 1], &st);
	/* Compute MD5 sum */
	md5_init(&state);
	md5_append(&state, (const md5_byte_t *) addr, st.st_size);
	md5_finish(&state, digest);
	/* Prepare output */
	for (di = 0; di < 16; ++di) {
		sprintf(hex_output + di * 2, "%02x", digest[di]);
	}
	printf("%s %s\n", hex_output, argv[argc - 1]);
	return 0;
}
