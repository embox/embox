/**
 * @file
 * @brief Implementation of #strncmp() function.
 *
 * @date 20.11.09
 * @author Nikolay Korotky
 */

#include <string.h>

int strncmp(const char *s1, const char *s2, size_t count) {
	if (!count)
		return 0;

	while (--count && *s1 && *s1 == *s2) {
		s1++;
		s2++;
	}

	return *(unsigned char *) s1 - *(unsigned char *) s2;
}
