/**
 * @file
 *
 * @date Aug 23, 2013
 * @author: Anton Bondarev
 */
#include <string.h>

char *strtok_r(char *str, const char *delim, char **saveptr) {
	char ch;

	if (str == NULL && (NULL == (str = *saveptr))) {
		return NULL;
	}

	/* search first not delimiting character */
	do {
		if ('\0' == (ch = *str++)) {
			return NULL;
		}
	} while(strchr(delim, ch));

	/* skip not delimiting characters */
	*saveptr = str + strcspn(str, delim);

	if (**saveptr != '\0') { /* if string is not finished */
		**saveptr = 0; /* we must insert zero instead of separator */
		(*saveptr)++;
	}

	return --str;
}

char *strtok(char *str, const char *delim) {
	static char *saveptr;
	return strtok_r(str, delim, &saveptr);
}
