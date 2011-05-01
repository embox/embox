/**
 * @file
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 */
#include <string.h>

void bcopy(const void *s, void *d, size_t n) {
	const unsigned char *src = (const unsigned char *) s;
	unsigned char *dest = (unsigned char *) d;
	if (dest < src) {
		while (n--) {
			*dest++ = *src++;
		}
	} else {
		const unsigned char *lasts = src + (n - 1);
		unsigned char *lastd = dest + (n - 1);
		while (n--)
			*(char *) lastd-- = *(char *) lasts--;
	}
}
