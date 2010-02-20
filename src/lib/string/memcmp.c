/**
 * @file
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 */
#include <string.h>

int memcmp(const void *dst, const void *src, size_t n) {
	if (!n) {
		return 0;
	}
	while (--n && *(char *) dst == *(char *) src) {
		dst = (char *) dst + 1;
		src = (char *) src + 1;
	}

	return *((unsigned char *) dst) - *((unsigned char *) src);
}
