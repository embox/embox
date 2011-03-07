/* This file is derived from the public domain implementation
 * by David MacKenzie <djm@gnu.ai.mit.edu>.  */

/**
 * @file
 * @brief Implementation of memmove and memcpy functions.
 *
 * @date 23.11.09
 * @author Eldar Abusalimov
 */

#include <string.h>

void *memcpy(void *dst, const void *src, size_t n) {
	return memmove(dst, src, n);
}

void *memmove(void *_dst, const void *_src, size_t length) {
	char *dst = _dst;
	const char *src = _src;

	if (src > dst) {
		/* Moving from hi mem to low mem; start at beginning.  */
		while (length--) {
			*dst++ = *src++;
		}

	} else if (src != dst) {
		/* Moving from low mem to hi mem; start at end.  */
		src += length;
		dst += length;
		while (length--) {
			*--dst = *--src;
		}

	}

	return _dst;
}

