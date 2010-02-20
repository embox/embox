/**
 * @file
 *
 * @date 01.10.09
 * @author Nikolay Korotky
 */
#include <string.h>

char *strrchr (const char *s, int c) {
	const char *found, *p;
	c = (unsigned char) c;

	if (c == '\0')
		return strchr (s, '\0');

	found = NULL;
	while ((p = strchr (s, c)) != NULL) {
		found = p;
		s = p + 1;
	}

	return (char *) found;
}
