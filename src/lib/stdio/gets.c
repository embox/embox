/**
 * @file
 * @brief TODO
 *
 * @date 14.02.10
 * @author Eldar Abusalimov
 */

#include <stdio.h>

char * fgets(char *s, int n, FILE *file) {
	int c = EOF; // has to add it, since compiler claims it's uninited --Anton Kozlov
		     // if 'while' below not taken, c in test for EOF really undefined
	char *ptr;

	if (n <= 0) {
		return NULL;
	}

	ptr = s;
	while ((--n > 0) && ((c = getc(file)) != EOF)) {
		*ptr++ = c;
		if (c == '\n') {
			break;
		}
	}

	if ((c == EOF) && (ptr == s)) {
		return NULL;
	}

	*ptr = '\0';

	return s;
}

char *gets(char *s) {
	int c;
	char *ptr;

	if (EOF == (c = getchar())) {
		return NULL;
	}
	for (ptr = s; c != '\n' && c != EOF; c = getchar()) {
		*ptr++ = c;
	}
	*ptr = 0;

	return s;
}
