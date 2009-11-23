/**
 * \file memmove.c
 * \date 23.11.09
 * \author Sikmir
 */
#include "string.h"

void *memmove( void *dst, const void *src, size_t n) {
	bcopy (src, dst, n);
        return dst;
}
