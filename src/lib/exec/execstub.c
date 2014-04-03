/**
 * exacstub.c
 *
 *  Created on: Feb 17, 2014
 *      Author: Anton Bondarev
 */

#include <unistd.h>
#include <stdio.h>

int execve(const char *filename, char *const argv[], char *const envp[]) {
	(void)filename;
	(void)argv;
	(void)envp;

	printf(">>> execve(%s,..,..)\n", filename);
	return -1;
}
