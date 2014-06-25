/*
 * stubs.c
 *
 *  Created on: 2014 6 24
 *      Author: alexander
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#if 1
#define DPRINT() printf(">>> qpid CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

int open64(const char *pathname, int oflag,...) {
	va_list args;
	int res;

	DPRINT();

	va_start(args, oflag);
	res = open(pathname, oflag, args);
	va_end(args);

	return res;
}

int syscall(int number, ...) {
	DPRINT();
	return -1;
}

long int jrand48(unsigned short xsubi[3]) {
	DPRINT();
	return random();
}

int access(const char *pathname, int mode) {
	DPRINT();
	return 0;
}

int __sprintf_chk(char * str, int flag, size_t strlen, const char * format, ...) {
	DPRINT();
	return -1;
}

int __fprintf_chk(FILE * stream, int flag, const char * format, ...) {
	DPRINT();
	return -1;
}

void __stack_chk_fail(void) {
	DPRINT();
}

int * __errno_location(void) {
	DPRINT();
	return NULL;
}

void __xstat64(void) {
	DPRINT();
}

int ftruncate64(int fd, off_t length) {
	DPRINT();
	return -1;
}

const unsigned short * * __ctype_b_loc (void) {
	DPRINT();
	return 0;
}

int fork(void) {
	DPRINT();
	return -1;
}
