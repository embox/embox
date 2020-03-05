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

#define IO_EOF_		   	0x0010  /* To check if EOF is seen*/
#define IO_ERR_		   	0x0020  /* To check if an error is seen*/
#define SET_IO_EOF(fp)		((fp)->flags |= IO_EOF_) /*Sets the field 0x0010*/
#define SET_IO_ERR(fp)		((fp)->flags |= IO_ERR_) /*Sets the field 0x0020*/
#define IO_EOF_DETECT(fp)		(((fp)->flags & IO_EOF_) != 0 ? 1:0) /*True if 0x0010 is set*/
#define IO_ERR_DETECT(fp)		(((fp)->flags & IO_ERR_) != 0 ? 1:0) /*True if 0x0020 is set*/
#define IO_CLEARERR(fp)		((fp)->flags &= ~(IO_ERR_ | IO_EOF_))

#endif /* STDIO_FILE_STRUCT_H_ */
