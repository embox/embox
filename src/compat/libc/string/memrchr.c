/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.11.2014
 */

#include <string.h>

void *memrchr(const void *s, int c, size_t n) {
	const unsigned char *src = (const unsigned char *) s + n - 1;
	unsigned char d = c;

	do {
		if (*src == d) {
			return (void *) src;
		}
	} while (src-- != s);

	return NULL;
}

