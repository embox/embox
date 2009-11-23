/**
 * \file string.c
 * \brief Contains some utils function for working with strings and memory
 * \date Mar 23, 2009
 * \author Anton Bondarev
 */
#include "string.h"

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
