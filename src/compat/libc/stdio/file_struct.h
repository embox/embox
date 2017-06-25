/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.11.2014
 */

#ifndef STDIO_FILE_STRUCT_H_
#define STDIO_FILE_STRUCT_H_

#include <stdio.h>

struct file_struct {
	int fd;
	int flags;

	int (*readfn)(void *, char *, int);
	int (*writefn)(void *, const char *, int);
	fpos_t (*seekfn)(void *, fpos_t, int);
	int (*closefn)(void *);
	const void *cookie;
	char has_ungetc;
	int ungetc;

	int buftype;
	void *obuf;
	int obuf_sz;
	int obuf_len;
};

extern int funopen_check(FILE *f);

#endif /* STDIO_FILE_STRUCT_H_ */
