/**
 * @file
 * @brief mblen function.
 *
 * @see stdlib.h
 *
 * @date 01.11.18
 * @author Chubukov Filipp
 */

#include <stdlib.h>

int mblen(const char *str, size_t max) {
	int n = 5;
	unsigned int ucs;
	if (str == NULL) {
		return 0;
	}

	ucs = *str & 0xFF;
	if (ucs < 128) {
		return 1;
	}
	if ((ucs & 0xC0) == 0x80 || ucs == 0xFF || ucs == 0xFE) {
		return -1;
	}

	/* Depending on the number of bytes,
	*  the character in utf-8 looks like 0xxxxxxx or 110xxxxx or 1110xxxx or  
	* 11110xxx and here we check for it 1 << 5 = 100000 & 110xxxxx ....
	*/
	while (n && (ucs & (1 << n))) {
		n--;
	}

	if (7-n > max) {
		return max;
	} else {
		return 7-n;
	}
}
