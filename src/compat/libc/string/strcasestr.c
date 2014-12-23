/**
 * @file
 * @brief Implementation of #strcasestr() function.
 *
 * @date    23.12.2014
 * @author  Anton Kozlov
 *         - Derived from #strstr() implementation
 */

#include <ctype.h>
#include <string.h>

/* NOTE #strcasestr is #strstr with #tolower calls. If you change this,
 * change #strstr() too
 */
char *strcasestr(const char *haystack, const char *needle) {
	if (!*needle) {
		return (char *) haystack;
	}

	for (; *haystack; ++haystack) {
		const char *h = haystack, *n = needle;

		while (*h && tolower(*h) == tolower(*n)) {
			++h;
			++n;
		}

		if (!*n) {
			return (char *) haystack;
		}
	}

	return NULL;
}
