/**
 * \file memcpy.c
 * \date 14.10.09
 * \author Sikmir
 */
#include "string.h"

void *memcpy(void *dst, const void *src, size_t n) {
	void *ret = dst;

	while (n--) {
		*(char *) dst = *(char *) src;
		dst = (char *) dst + 1;
		src = (char *) src + 1;
	}

	return ret;
}
