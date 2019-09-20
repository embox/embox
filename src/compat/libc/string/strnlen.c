/**
 * @file
 *
 * @date Aug 29, 2013
 * @author: Anton Bondarev
 */

#include <string.h>

size_t strnlen(const char *str, size_t maxlen) {
	size_t len;
	const char *s = str;

	for (len = 0; len < maxlen; len++) {
		if ('\0' == *s++) {
			break;
		}
	}

	return len;
}
