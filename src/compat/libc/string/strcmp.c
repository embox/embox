/**
 * @file
 * @brief Implementation of #strcmp() function.
 *
 * @date 13.10.09
 * @author Nikolay Korotky
 */

#include <string.h>

int strcmp(const char *str1, const char *str2) {
	unsigned const char *s1 = (unsigned const char *) str1;
	unsigned const char *s2 = (unsigned const char *) str2;

	while (*s1 && *s1 == *s2) {
		++s1;
		++s2;
	}

	return *s1 - *s2;
}
