/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.05.2014
 */

#include <stdio.h>
#include <string.h>
#include <lib/crypt/md5.h>

#include <crypt.h>

char *crypt(const char *key, const char *salt) {
	md5_state_t state;
	md5_byte_t digest[16*2 + 1];
	static char hex_output[16*2 + 1];

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *) key, strlen(key));
	md5_finish(&state, digest);

	for (int i = 0; i < 16; i++) {
		sprintf(hex_output + i * 2, "%02x", digest[i]);
	}

	return hex_output;
}
