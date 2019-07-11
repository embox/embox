/**
 * @file
 * @brief strcoll() function compare two strings using C locale.
 *
 * @date   09.07.19
 * @author Nastya Nizharadze
 */

#include <string.h>
#include <stdio.h>
#include <assert.h>

#define IS_NOT_ASCII (1u << 7)

int strcoll(const char *s1, const char *s2) {

	if (((*s1) & (IS_NOT_ASCII)) || ((*s2) & IS_NOT_ASCII)) {
		printf("strcoll: error: not ASCII character\n");
		assert(0);
	}
	while (*s1 && *s1 == *s2) {
		++s1;
		++s2;
		if (((*s1) & (IS_NOT_ASCII)) || ((*s2) & IS_NOT_ASCII)) {
			printf("strcoll: error: not ASCII character\n");
			assert(0);
		}
	}

	return *s1 - *s2;
}
