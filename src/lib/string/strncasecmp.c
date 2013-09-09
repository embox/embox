/**
 * @file
 *
 * @date Aug 23, 2013
 * @author: Anton Bondarev
 */

#include <ctype.h>
#include <strings.h>

int strncasecmp(const char *str1, const char *str2, size_t n) {
	const unsigned char *s1 = (const unsigned char *) str1;
	const unsigned char *s2 = (const unsigned char *) str2;

	if (!n) {
		return 0;
	}

	while (--n && *s1 && (tolower(*s1) == tolower(*s2))) {
		++s1;
		++s2;
	}

	return tolower(*s1) - tolower(*s2);
}
