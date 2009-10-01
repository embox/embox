/**
 * \file string.c
 * \brief Contains some utils function for working with strings and memory
 * \date Mar 23, 2009
 * \author Anton Bondarev
 */
#include "string.h"

char ch_upcase(char ch) {
	if (ch >= 'a' && ch <= 'z')
		return (ch + 'A' - 'a');
	else
		return ch;
}

int ch_to_digit(char ch, int base) {
	ch = ch_upcase(ch);
	switch (base) {
	case 0x10: {
		if (ch >= '0' && ch <= '9') {
			return (ch - '0');
		} else if (ch >= 'A' && ch <= 'F') {
			return (ch - 'A' + 0x0A);
		}
		return -1;
	}
	case 10: {
		if (ch >= '0' && ch <= '9') {
			return (ch - '0');
		}
		return -1;
	}
	case 8: {
		if (ch >= '0' && ch <= '7') {
			return (ch - '0');
		}
		return -1;
	}
	default:
		return -1;
	}
	return -1;
}

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

int strcmp(const char * str1, const char * str2) {
	int ret = 0;
	while (!(ret = *(unsigned char *) str1 - *(unsigned char *) str2) && *str2)
		++str1, ++str2;

	if (ret < 0)
		ret = -1;
	else if (ret > 0)
		ret = 1;

	return ret;
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

int is_digit(int ch, int base) {
	ch = (int) ch_upcase((char) ch);
	switch (base) {
	case 10: {
		if ((ch >= '0') && (ch <= '9'))
			return TRUE;
		return FALSE;
	}
	case 8: {
		if ((ch >= '0') && (ch <= '7'))
			return TRUE;
		return FALSE;
	}
	case 16: {
		if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')))
			return TRUE;
		return FALSE;
	}
	default:
		return FALSE;
	}
	return FALSE;
}

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

void *memcpy(void *dst, const void *src, size_t n) {
	void *ret = dst;

	while (n--) {
		*(char *) dst = *(char *) src;
		dst = (char *) dst + 1;
		src = (char *) src + 1;
	}

	return ret;
}
