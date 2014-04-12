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

#include <sys/ioctl.h>

#include <mem/misc/pool.h>

#include <framework/mod/options.h>



#include <fs/kfile.h>


#define FILE_QUANTITY OPTION_GET(NUMBER,file_quantity)

#define DEFAULT_MODE 0755

POOL_DEF(file_pool, FILE, FILE_QUANTITY);

static FILE *file_alloc(int fd) {
	FILE *file = pool_alloc(&file_pool);

	if (!file) {
		return NULL;
	}

	file->fd = fd;
	file->has_ungetc = 0;
	return file;
}

static void file_free(FILE *file) {
	if ((file != stdin) && (file != stdout)
			&& (file != stderr)) {
		pool_free(&file_pool, file);
	}
}

FILE *fopen(const char *path, const char *mode) {
	int fd;
	FILE *file = NULL;
	int flags = 0;

	if (strchr(mode, 'r')) {
		flags |= O_RDONLY;
	}

	if (strchr(mode, 'w')) {
		flags |= O_WRONLY | O_TRUNC | O_CREAT;
	}

	if (strchr(mode, 'a')) {
		flags |= O_APPEND | O_WRONLY | O_CREAT;
	}

	if ((fd = open(path, flags, DEFAULT_MODE)) < 0) {
		/* That's sad, but open sets errno, no need to alter */
		return NULL;
	}

	if (NULL == (file = file_alloc(fd))) {
		close(fd);
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	return file;

}

FILE *fdopen(int fd, const char *mode) {
	/**
	 * FIXME mode ignored now
	 */
	FILE *file;

	file = file_alloc(fd);
	if (file == NULL) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	return file;
}

FILE *freopen(const char *path, const char *mode, FILE *file) {
	if (NULL == file) {
		return file;
	}
	return NULL;
}

int fclose(FILE *file) {
	int res;

	if (NULL == file){
		SET_ERRNO(EBADF);
		return -1;
	}

	res = close(file->fd);
	if (res >= 0) {
		file_free(file);
	}

	return res;
}

