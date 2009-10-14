/**
 * \file string.c
 * \brief Contains some utils function for working with strings and memory
 * \date Mar 23, 2009
 * \author Anton Bondarev
 */
#include "string.h"

int strlen(const char * str) {
	const char *eos = str;

	while (*eos++)
		;

	return (int) (eos - str - 1);
}

char *strcpy(char * dest, const char * src) {
	char *cp = dest;
	while ((*cp++ = *src++))
		;
	return dest;
}

char *strncpy(char *dest, const char *source, size_t count) {
	char *start = dest;
	while (count && (*dest++ = *source++))
		count--;
	while (count--)
		*dest++ = '\0';
	return start;
}

int strncmp(const char *s1, const char *s2, size_t count) {
	if (!count)
		return 0;

	while (--count && *s1 && *s1 == *s2) {
		s1++;
		s2++;
	}

	return *(unsigned char *) s1 - *(unsigned char *) s2;
}

/*int str_starts_with(const char *str, const char *beg, int len) {
	// TODO change to strncpy
	int i;
	for (i = 0; *beg == *str || *beg == 0 || i == len; beg++, str++, i++) {
		if (*beg == 0 || i == len) {
			return TRUE;
		}
	}
	return FALSE;
}*/

int memcmp(const void *dst, const void *src, size_t n) {
	if (!n)
		return 0;

	while (--n && *(char *) dst == *(char *) src) {
		dst = (char *) dst + 1;
		src = (char *) src + 1;
	}

	return *((unsigned char *) dst) - *((unsigned char *) src);
}

void *memset(void *p, int c, size_t n) {
	char *pb = (char *) p;
	char *pbend = pb + n;
	while (pb != pbend)
		*pb++ = c;
	return p;
}

char *basename(const char *filename) {
	char *p = strrchr(filename, '/');
	return p ? p + 1 : (char *) filename;
}

// *str becomes pointer to first non-space character
void skip_spaces(char **str) {
        while (**str == ' ') {
                (*str)++;
        }
}

// *str becomes pointer to first space or '\0' character
void skip_word(char **str) {
        while (**str != '\0' && **str != ' ') {
                (*str)++;
        }
}
