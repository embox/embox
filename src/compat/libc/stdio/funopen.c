/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.07.2014
 */

#include <stdio.h>

extern FILE *stdio_file_alloc(int fd);

FILE *funopen(const void *cookie,
		int (*readfn)(void *, char *, int),
		int (*writefn)(void *, const char *, int),
		fpos_t (*seekfn)(void *, fpos_t, int),
		int (*closefn)(void *)) {
	FILE *file;

	if ((file = stdio_file_alloc(0))) {
		file->readfn = readfn;
		file->cookie = cookie;
	}

	return file;
}

