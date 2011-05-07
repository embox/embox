/**
 * @file
 * @brief Implements puts function.
 *
 * @date 14.02.10
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
