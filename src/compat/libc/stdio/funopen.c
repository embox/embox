/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.07.2014
 */

#include <stdio.h>
#include "file_struct.h"

extern FILE *stdio_file_alloc(int fd);

#define FUNOPEN_FD_MARK (-1)
FILE *funopen(const void *cookie,
		int (*readfn)(void *, char *, int),
		int (*writefn)(void *, const char *, int),
		fpos_t (*seekfn)(void *, fpos_t, int),
		int (*closefn)(void *)) {
	FILE *file;

	if ((file = stdio_file_alloc(FUNOPEN_FD_MARK))) {
		file->readfn = readfn;
		file->writefn = writefn;
		file->closefn = closefn;
		file->seekfn = seekfn;
		file->cookie = cookie;
	}

	return file;
}

int funopen_check(FILE *f) {
	return f->fd == FUNOPEN_FD_MARK;
}
