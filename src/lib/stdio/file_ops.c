/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <errno.h>
#include <string.h>
#include <mem/misc/pool.h>

#include <framework/mod/options.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <fs/kfile.h>
#include <stdio.h>

#define FILE_QUANTITY OPTION_GET(NUMBER,file_quantity)

#define DEFAULT_MODE 0755

POOL_DEF(file_pool, FILE, FILE_QUANTITY);

FILE stdin_struct = {
	.fd = STDIN_FILENO,
};
FILE stdout_struct = {
	.fd = STDOUT_FILENO,
};

FILE stderr_struct = {
	.fd = STDERR_FILENO,
};

FILE *stdin = &stdin_struct;
FILE *stdout = &stdout_struct;
FILE *stderr = &stderr_struct;

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

	if ((fd = open(path, flags, DEFAULT_MODE)) > 0) {
		file = pool_alloc(&file_pool);
		file->fd = fd;
	}

	return file;

}

FILE *freopen(const char *path, const char *mode, FILE *file) {
	if (NULL == file) {
		return file;
	}
	return NULL;
}

int feof(FILE *file) {
	//TODO:
	return 0;
}

int ferror(FILE *file) {
	//TODO:
	return 0;
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	if (NULL == file) {
		return -1;
	}
	return write(file->fd, buf, size * count);
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	char *cbuff;
	uint addon = 0;

	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (file->has_ungetc) {
		file->has_ungetc = 0;
		cbuff = buf;
		cbuff[0] = (char)file->ungetc;
		count --;
		buf = &cbuff[1];
		addon = 1;
	}

	return (addon + read(file->fd,  buf, size * count));
}

int fclose(FILE *file) {
	int res;

	if (NULL == file){
		SET_ERRNO(EBADF);
		return -1;
	}
	res = close(file->fd);

	if (res >= 0) {
		pool_free(&file_pool, file);
	}
	return res;
}

int fseek(FILE *file, long int offset, int origin) {
	off_t ret;

	if (origin != SEEK_SET && origin != SEEK_CUR
			&& origin != SEEK_END) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = lseek(file->fd, offset, origin);
	if (ret == (off_t)-1) {
		return -1;
	}

	return 0;
}

long int ftell(FILE *file) {
	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}
	return lseek(file->fd, 0L, SEEK_CUR);
}

void rewind(FILE *file) {
	fseek(file, 0L, SEEK_SET);
}

int fioctl(FILE *file, int request, ...) {
	va_list args;
	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}
	va_start(args, request);
	return ioctl(file->fd, request, args);
}

int ungetc(int ch, FILE *file) {
	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}
	file->ungetc = (char) ch;
	file->has_ungetc = 1;
	return ch;
}
