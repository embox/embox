/**
 * @file
 * @brief Implementation of #memcmp() function.
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 */

#include <string.h>

int memcmp(const void *_dst, const void *_src, size_t n) {
	const unsigned char *dst = (const unsigned char *) _dst;
	const unsigned char *src = (const unsigned char *) _src;

	if (!n) {
		return 0;
	}

	while (--n && *dst == *src) {
		++dst;
		++src;
	}

	return *dst - *src;
}
