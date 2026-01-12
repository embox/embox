/**
 * @file
 *
 * @date Aug 23, 2013
 * @author: Anton Bondarev
 */

#include <ctype.h>
#include <strings.h>

int strcasecmp(const char *str1, const char *str2) {
	unsigned const char *s1 = (unsigned const char *) str1;
	unsigned const char *s2 = (unsigned const char *) str2;

	while (*s1 && (tolower(*s1) == tolower(*s2))) {
		++s1;
		++s2;
	}

	return tolower(*s1) - tolower(*s2);
}
