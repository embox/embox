#include "types.h"

// character to upper
char ch_upcase(char ch) {
	if (ch >= 'a' && ch <= 'z')
		return (ch + 'A' - 'a');
	else
		return ch;
}

// convert HEX digit character to integer
int ch_to_digit(char ch, int base) {
	ch = ch_upcase(ch);
	switch (base)
	{
		case 0x10:
		{
			if (ch >= '0' && ch <= '9') {
				return (ch - '0');
			} else if (ch >= 'A' && ch <= 'F') {
				return (ch - 'A' + 0x0A);
			}
			return -1;
		}
		case 10:
		{
			if (ch >= '0' && ch <= '9') {
				return (ch - '0');
			}
			return -1;
		}
		case 8:
		{
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
	int i;
	for (i = 0; str[i]; i++)
		;
	return i;
}

int strcpy(char * dest, const char * src) {
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

int strcmp(const char * str1, const char * str2) {
//	int ret = 0;
//	while (!(ret = *(unsigned char *) str1 - *(unsigned char *) str2) && *str2)
//		++str1, ++str2;
//
//	if (ret < 0)
//		ret = -1;
//	else if (ret > 0)
//		ret = 1;
//
//	return ret;
	for (; *str1 == *str2; str1++, str2++) {
		if (*str1 == 0)
			return TRUE;
	}
	return FALSE;
}




WORD atow(char *buff) {
	WORD result = 0;
	WORD paw = 1;
	int length = strlen(buff) - 1;
	for (; length >= 0; length--) {
		result += ch_to_digit(buff[length], 0x10) * paw;
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

BOOL is_digit(int ch, int base) {
	ch = (int)ch_upcase((char)ch);
	switch (base) {
		case 10:
		{
			if ((ch >= '0') && (ch <= '9'))
				return TRUE;
			return FALSE;
		}
		case 8:
		{
			if ((ch >= '0') && (ch <= '7'))
				return TRUE;
			return FALSE;
		}
		case 16:
		{
			if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')))
				return TRUE;
			return FALSE;
		}
		default:
			return FALSE;
	}
	return FALSE;
}
