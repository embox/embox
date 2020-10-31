/**
 * @file
 *
 * @date   30.10.2020
 * @author Alexander Kalmuk
 */

#include <stdio.h>

FILE *popen(const char *command, const char *type) {
	(void) command;
	(void) type;

	return NULL;
}

int pclose(FILE *stream) {
	(void) stream;

	return 0;
}
