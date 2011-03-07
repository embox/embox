/**
 * @file
 * @brief Implementation of #strcpy() function.
 *
 * @date 20.11.09
 * @author Nikolay Korotky
 */

#include <string.h>

char *strcpy(char * dest, const char * src) {
	char *cp = dest;
	while ((*cp++ = *src++))
		;
	return dest;
}
