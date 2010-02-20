/**
 * @file
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 */
#include <string.h>

char *strpbrk (const char *s, const char *accept) {
	while (*s != '\0') {
		const char *a = accept;
		while (*a != '\0')
			if (*a++ == *s)
				return (char *) s;
		++s;
	}

	return NULL;
}
