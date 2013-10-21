/**
 * @file
 * @brief Implementation of #memchr() function.
 *
 * @date 10.11.11
 * @author Nikolay Korotkiy
 */

#include <string.h>

void *memchr(const void *s, int c, size_t n) {
	const unsigned char *src = (const unsigned char *) s;
	unsigned char d = c;

	while (n--) {
		if (*src == d)
			return (void *) src;
		src++;
	}

	return NULL;
}
