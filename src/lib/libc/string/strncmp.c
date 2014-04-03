/**
 * @file
 * @brief Implementation of #strncmp() function.
 *
 * @date 20.11.09
 * @author Nikolay Korotky
 */

#include <string.h>

int strncmp(const char *str1, const char *str2, size_t n) {
	const unsigned char *s1 = (const unsigned char *) str1;
	const unsigned char *s2 = (const unsigned char *) str2;

	if (!n) {
		return 0;
	}

	while (--n && *s1 && *s1 == *s2) {
		++s1;
		++s2;
	}

	return *s1 - *s2;
}
