/**
 * @file
 * @brief Implements puts function.
 *
 * @date 14.02.10
 * @author Eldar Abusalimov
 */

#include <stdio.h>

int puts(const char *s) {
	if (EOF == fputs(s, stdout)) {
		return EOF;
	}

	return putchar('\n');
}

int fputs(const char *s, FILE *f) {
	while (*s != '\0') {
		if (EOF == fputc(*s++, f)) {
			return EOF;
		}
	}

	return 0;
}
