/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "term_priv.h"

#define NUM_PARM 9

char *tparm(char *str, long p1, long p2, long p3, long p4, long p5, long p6,
    long p7, long p8, long p9) {
	static char result[64];
	long parms[NUM_PARM];
	char number[20];
	char *ptr = result;
	long cur_parm = p1;

	if (str == NULL) {
		return NULL;
	}

	parms[0] = p1;
	parms[1] = p2;
	parms[2] = p3;
	parms[3] = p4;
	parms[4] = p5;
	parms[5] = p6;
	parms[6] = p7;
	parms[7] = p8;
	parms[8] = p9;

	for (; *str; str++) {
		if (*str != '%') {
			*ptr++ = *str;
			continue;
		}

		str++;
		switch (*str) {
		case 'd':
			sprintf(number, "%ld", cur_parm);
			strcpy(ptr, number);
			ptr += strlen(number);
			break;

		case 'i':
			parms[0]++;
			parms[1]++;
			break;

		case 'p':
			str++;
			cur_parm = parms[*str - '1'];
		}
	}

	*ptr = '\0';

	return result;
}
