/**
 * \file memset.c
 * \date 23.11.09
 * \author Sikmir
 */
#include "string.h"

void *memset(void *p, int c, size_t n) {
        char *pb = (char *) p;
        char *pbend = pb + n;
        while (pb != pbend)
        	*pb++ = c;
        return p;
}
