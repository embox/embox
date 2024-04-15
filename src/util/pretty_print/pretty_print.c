/**
 * @file pretty_print.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.10.2019
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define STR_BYTES 16

size_t pretty_print_row_len(void) {
	return STR_BYTES;
}

/* Print up to STR_BYTES to buffer */
int pretty_print_row(void *buf, size_t len, char *dest) {
	size_t offt = 0;
	if (len > STR_BYTES) {
		len = STR_BYTES;
	}

	for (int i = 0; i < len; i++) {
		sprintf(dest, " %02hhX", *((uint8_t *) buf + offt));
		offt++;
		dest += 3; /* One space + two hex symbols */
	}

	sprintf(dest, "    ");
	dest += 4; /* Four spaces */

	for (int i = 0; i < len; i++) {
		char c = (char) *((uint8_t *) buf + i);
		if (c < 33 || c > 126) {
			c = '.';
		}

		sprintf(dest, "%c", c);
		dest++;
	}

	return 0;
}
