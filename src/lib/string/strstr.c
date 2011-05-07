/**
 * @file
 * @brief Implementation of #strstr() function.
 *
 * @date 18.10.09
 * @author Nikolay Korotky
 *         - Initial import
 * @author Eldar Abusalimov
 *         - Replacing with more simple and obvious implementation
 */

#include <string.h>

char *strstr(const char *haystack, const char *needle) {
	if (!*needle) {
		return (char *) haystack;
	}

	for (; *haystack; ++haystack) {
		const char *h = haystack, *n = needle;

		while (*h && *h == *n) {
			++h;
			++n;
		}

		if (!*n) {
			return (char *) haystack;
		}
	}

	return NULL;
}
