/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "term_priv.h"

int tputs(char *str, int affcnt, int (*putc)(int)) {
	bool mandatory;
	useconds_t number = 0;

	if (str == NULL) {
		return ERR;
	}

	while (*str) {
		if ((*str != '$') || (*(str + 1) != '<') || !strchr(str, '>')) {
			(*putc)(*str);
		}
		else {
			str += 2;
			while (isdigit(*str)) {
				number = number * 10 + (*str - '0');
				str++;
			}
			if (*str == '.') {
				while (isdigit(*str)) {
					str++;
				}
			}
			mandatory = FALSE;
			while (*str != '>') {
				if (*str == '*') {
					number *= affcnt;
				}
				else if (*str == '/') {
					mandatory = TRUE;
				}
				str++;
			}
			if ((number > 0) && mandatory) {
				usleep(number);
			}
		}
		str++;
	}

	return OK;
}

int putp(char *str) {
	return tputs(str, 1, putchar);
}
