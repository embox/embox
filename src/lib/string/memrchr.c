/**
 * \file memrchr.c
 * \date 25.11.09
 * \author Sikmir
 */
#include "string.h"

void *memrchr(const void *s, int c, size_t n) {
        const unsigned char *src = (const unsigned char *)s;

        for (src += n; n ; n--) {
    		if (*--src == c) {
                        return (void*)src;
                }
        }
        return NULL;
}
