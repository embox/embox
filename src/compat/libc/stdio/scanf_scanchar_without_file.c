/**
 * @file
 *
 * @date Nov 3, 2020
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <stdint.h>

void __stdio_unscanchar(const char **str, int ch, int *pc_ptr) {
	if (ch != EOF) {
		(*pc_ptr)--;
	}
	if ((uintptr_t) str >= 2) {
		(*str) --;
	}
}

int __stdio_scanchar(const char **str, int *pc_ptr) {
	int ch = 0;
	if ((uintptr_t)str >= 2) {
		ch = **str;
		(*str)++;
		ch = (ch == '\0' ? EOF : ch);
	}
	if (ch != EOF) {
		(*pc_ptr)++;
	}
	return ch;
}
