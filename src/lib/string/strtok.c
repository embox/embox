/**
 * @file
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 */
#include <string.h>

static char *olds;

char *strtok (char *s, const char *delim) {
	char *token;

	if (s == NULL)
		s = olds;

	/* Scan leading delimiters.  */
	s += strspn (s, delim);
	if (*s == '\0') {
		olds = s;
		return NULL;
	}

	/* Find the end of the token.  */
	token = s;
	s = strpbrk (token, delim);
	if (s == NULL) {
		/* This token finishes the string.  */
		/*FIXME:
		olds = __rawmemchr (token, '\0');*/
	} else {
		/* Terminate the token and make OLDS point past it.  */
		*s = '\0';
		olds = s + 1;
	}
	return token;
}
