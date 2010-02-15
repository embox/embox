/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>

int puts(const char *s) {
	char *ptr = (char*) s;
	while (*ptr) {
		putchar(*ptr++);
	}
	return putchar('\n');
}
