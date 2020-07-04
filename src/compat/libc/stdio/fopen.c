/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include "file_struct.h"

#include <sys/ioctl.h>
#include <sys/stat.h>

#define DEFAULT_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) /* 0666 */

extern FILE *stdio_file_alloc(int fd);

static int mode2flag(const char *mode) {
	int oflags, omode, loop;

	switch (*mode++) {
	case 'r':
		omode = O_RDONLY;
		oflags = 0;
		break;
	case 'w':
		omode = O_WRONLY;
		oflags = O_CREAT|O_TRUNC;
		break;
	case 'a':
		omode = O_WRONLY;
		oflags = O_CREAT|O_APPEND;
		break;
	default: /*illegal mode */
		SET_ERRNO(EINVAL);
		return -1;
	}

	do {
		loop = 1;
		switch (*mode++) {
		case 'b':
			/* ignore 'b' (binary) */
			break;
		case '+':
			omode = O_RDWR;
			break;
		default:
			loop = 0;
			break;
		}
	} while (loop);
	return omode | oflags;
}

FILE *fopen(const char *path, const char *mode) {
	int fd;
	FILE *file = NULL;
	int flags = 0;

	if((flags = mode2flag(mode)) < 0 ) {
		return NULL;
	}

	if ((fd = open(path, flags, DEFAULT_MODE)) < 0) {
		/* That's sad, but open sets errno, no need to alter */
		return NULL;
	}

	if (NULL == (file = stdio_file_alloc(fd))) {
		close(fd);
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	file->flags = flags;

	return file;

}

FILE *fdopen(int fd, const char *mode) {
	/**
	 * FIXME mode ignored now
	 */
	FILE *file;

	file = stdio_file_alloc(fd);
	if (file == NULL) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	return file;
}

FILE *freopen(const char *path, const char *mode, FILE *file) {
	int fd;
	int flags = 0;
	int old_fd;

	if (NULL == path || NULL == file) {
		SET_ERRNO(EINVAL);
		return NULL;
	}

	if((flags = mode2flag(mode)) < 0 ) {
		return NULL;
	}

	if ((fd = open(path, flags, DEFAULT_MODE)) < 0) {
		/* That's sad, but open sets errno, no need to alter */
		return NULL;
	}
	old_fd = file->fd;

	dup2(fd, old_fd);
	file->flags = flags;
	clearerr(file); /* redundant but just-in-case */

	close(fd);

	return file;
}
