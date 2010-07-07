/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <stdio.h>

int getchar(void) {
	return fgetc(STDIN);
}

int ungetchar(int ch) {
	return fungetc(STDIN,ch);
}

