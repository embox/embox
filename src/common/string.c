#include "types.h"

// character to upper
char ch_upper(char ch) {
	if (ch >= 'a' && ch <= 'z')
		return (ch + 'A' - 'a');
	else
		return ch;
}

// convert HEX digit character to integer
int ch_to_digit(char ch) {
	ch = ch_upper(ch);
	if (ch >= '0' && ch <= '9') {
		return (ch - '0');
	} else if (ch >= 'A' && ch <= 'F') {
		return (ch - 'A' + 0x0A);
	}
	return 0;
}

int sz_length(const char * str) {
	int i;
	for (i = 0; str[i]; i++)
		;
	return i;
}

int sz_cpy(char * dest, const char * src) {
	int i = 0;
	while (*src != '\0') {
		*dest++ = *src++;
		i++;
	}
	*dest = '\0';
	return i;
}

int sz_append(char *dest, const char *src, char ch) {
	int i;
	for (i = 0; src[i] != 0; i++)
		dest[i] = src[i];

	if (i > 0)
		dest[i++] = '.';

	dest[i++] = ch;
	dest[i] = 0x0;
	return i;
}

int sz_cmp(const char * str1, const char * str2) {
	for (; *str1 == *str2; str1++, str2++) {
		if (*str1 == 0)
			return TRUE;
	}
	return FALSE;
}

WORD atow(char *buff) {
	WORD result = 0;
	WORD paw = 1;
	int length = sz_length(buff) - 1;
	for (; length >= 0; length--) {
		result += ch_to_digit(buff[length]) * paw;
		paw = paw * 0x10;
	}
	return result;
}

// Determines whether beg is the beginning of the str string
int sz_cmp_beginning(const char *beg, const char *str){
	for (; *beg == *str || *beg == 0; beg++, str++) {
		if (*beg == 0)
			return TRUE;
	}
	return FALSE;
}
