/**
 * \file strcmp.c
 * \date 13.10.09
 * \author Sikmir
 */
#include "string.h"

int strcmp(const char *str1, const char *str2) {
	const unsigned char *s1 = (const unsigned char *) str1;
	const unsigned char *s2 = (const unsigned char *) str2;
	unsigned char c1, c2;

	 do {
		c1 = (unsigned char) *s1++;
	        c2 = (unsigned char) *s2++;
	        if (c1 == '\0')
	    		return c1 - c2;
	} while (c1 == c2);

	return c1 - c2;
}
