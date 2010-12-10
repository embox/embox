/**
 * @file
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 */
#include <string.h>

size_t strspn(const char *s, const char *accept) {
	const char *p;
	const char *a;
	size_t count = 0;

	for (p = s; *p != '\0'; ++p) {
		for (a = accept; *a != '\0'; ++a)
			if (*p == *a)
				break;
			if (*a == '\0')
				return count;
			else
				++count;
	}
	return count;
}
