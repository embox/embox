/**
 * @file
 * @brief Compute and check MD5 message digest.
 *
 * @date 10.03.10
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>
#include <lib/md5.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: md5sum [FILE]\n");
}

static int exec(int argc, char **argv) {
	FILE *fd;
	stat_t st;
	uint32_t addr;
	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;
	/* Get size and file's base addr */
	fd = fopen(argv[argc - 1], "r");
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

