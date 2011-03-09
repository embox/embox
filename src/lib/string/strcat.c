/**
 * @file
 * @brief Implementation of #strcat() function.
 *
 * @date 13.10.09
 * @author Nikolay Korotky
 */
#include <string.h>

char *strcat(char *dest, const char *src) {
	char *s1 = dest;
	const char *s2 = src;
	char c;

	do {
		c = *s1++;
	} while (c != '\0');

	s1 -= 2;
	do {
		c = *s2++;
		*++s1 = c;
	} while (c != '\0');
	return dest;
}
