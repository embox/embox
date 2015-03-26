/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.06.2014
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	int ecode = 0;

	if ((0 == strcmp(argv[0], "exit")) && argc != 1) {
		char *eptr;

		ecode = strtol(argv[1], &eptr, 0);
		if (*eptr != '\0') {
			ecode = 255;
			fprintf(stderr, "exit: exit code should be numerical value\n");
		}
	}

	exit(ecode);
}
