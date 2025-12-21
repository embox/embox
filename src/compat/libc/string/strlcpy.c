/**
 * @file
 *
 * @date Aug 28, 2013
 * @author: Anton Bondarev
 */

#include <stddef.h>
#include <string.h>

/*
 * Copy src to string dst of size siz. At most siz-1 characters will be copied.
 * Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t strlcpy(char *dst, const char *src, size_t size) {
	size_t n = size;
	const char *s = src;

	if (size == 0) {
		return strlen(src);
	}

	while (n-- != 1) {
		if (*s == '\0') {
			break;
		}
		*dst++ = *s++;
	}

	*dst = '\0';

	return s - src;
}
