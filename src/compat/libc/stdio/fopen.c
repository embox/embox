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

#define DEFAULT_MODE 0666

extern FILE *stdio_file_alloc(int fd);

static int mode2flag(const char *mode) {
	int flags = 0;

	if (strchr(mode, 'r') && strchr(mode, 'w')) {
		flags |= O_RDWR | O_TRUNC | O_CREAT;
	} else if (strchr(mode, 'w')) {
		flags |= O_WRONLY | O_TRUNC | O_CREAT;
	}

	if (strchr(mode, 'a')) {
		flags |= O_APPEND | O_CREAT | O_RDWR;
	}

	return flags;
}

FILE *fopen(const char *path, const char *mode) {
	int fd;
	FILE *file = NULL;
	int flags = 0;

	flags = mode2flag(mode);

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
		return NULL;
	}

	flags = mode2flag(mode);

	if ((fd = open(path, flags, DEFAULT_MODE)) < 0) {
		/* That's sad, but open sets errno, no need to alter */
		return NULL;
	}
	old_fd = file->fd;

	dup2(fd, old_fd);
	file->flags = flags;

	close(fd);

	return file;
}
