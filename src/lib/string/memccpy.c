/**
 * @file
 *
 * @date 14.10.09
 * @author Nikolay Korotky
 */
#include <string.h>

void *memccpy (void *dest, const void *src, int c, size_t n) {
	const char *s = src;
	char *d = dest;
	const char x = c;
	size_t i = n;

	while (i-- > 0)
		if ((*d++ = *s++) == x)
			return d;

	return NULL;
}
