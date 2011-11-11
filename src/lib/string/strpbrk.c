/**
 * @file
 * @brief Implementation of #strpbrk() function.
 *
 * @date 10.11.11
 * @author Nikolay Korotkiy
 */

#include <string.h>

char *strpbrk(const char *s1, const char *s2) {
	const  char *c = s2;
	if (!*s1) {
		return (char *) NULL;
	}

	while (*s1) {
		for (c = s2; *c; c++) {
			if (*s1 == *c)
				break;
		}
		if (*c)
			break;
		s1++;
	}

	if (*c == '\0')
		s1 = NULL;

	return (char *) s1;
}
