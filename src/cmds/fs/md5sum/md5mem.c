#include <stdio.h>
#include <stdlib.h>
#include <lib/crypt/md5.h>

int main(int argc, char *argv[]) {
	md5_state_t state;
	md5_byte_t digest[16];
	int len;
	uint8_t *addr;
	int di;
	char hex_output[16 * 2 + 1];

	if (argc != 3) {
		printf("%s [addr] [len]\n", argv[0]);
		return 0;
	}

	addr = (uint8_t *) strtoul(argv[1], 0, 0);
	len  = atoi(argv[2]);

	printf("addr=%p, len=%d\n", addr, len);

	md5_init(&state);
	{
		md5_append(&state, (const md5_byte_t *) addr, len);
	}
	md5_finish(&state, digest);

	/* Prepare output */
	for (di = 0; di < 16; ++di) {
		sprintf(hex_output + di * 2, "%02x", digest[di]);
	}
	printf("%s\n", hex_output);

	return 0;
}
