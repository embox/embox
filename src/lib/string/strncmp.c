/**
 * @file
 * @brief Implementation of #strncmp() function.
 *
 * @date 20.11.09
 * @author Nikolay Korotky
 */

#include <string.h>

int strncmp(const char *str1, const char *str2, size_t n) {
	unsigned const char *s1 = (unsigned const char *) str1;
	unsigned const char *s2 = (unsigned const char *) str2;

	if (!n) {
		return 0;
	}

	while (--n && *s1 && *s1 == *s2) {
		++s1;
		++s2;
	}

	return *s1 - *s2;
}
