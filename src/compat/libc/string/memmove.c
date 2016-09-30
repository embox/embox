/* This file is derived from the public domain implementation
 * by David MacKenzie <djm@gnu.ai.mit.edu>.  */

/**
 * @file
 * @brief Implementation of #memmove() function, delegates to #memcpy().
 *
 * @date 23.11.09
 * @author Eldar Abusalimov
 */

#include <string.h>

#include "inhibit_libcall.h"

inhibit_loop_to_libcall
void *memmove(void *_dst, const void *_src, size_t n) {
	char *dst = _dst;
	const char *src = _src;

	if (src < dst && dst < src + n) {
		/* Moving from low mem to hi mem; start at end.  */
		src += n;
		dst += n;
		while (n--) {
			*--dst = *--src;
		}
		return _dst;
	}

	return memcpy(_dst, _src, n);
}
