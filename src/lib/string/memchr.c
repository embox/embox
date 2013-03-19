/**
 * @file
 * @brief Implementation of #memchr() function.
 *
 * @date 10.11.11
 * @author Nikolay Korotkiy
 */

#include <string.h>

void *memchr(const void *s, int c, size_t n) {
	unsigned char d = c;

	while (n--) {
		if (*s == d)
			return (void *) s;
		s++;
	}
	return NULL;
}
