/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <errno.h>
#include <mem/misc/pool.h>

#include <framework/mod/options.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <fs/core.h>
#include <stdio.h>

#define FILE_QUANTITY OPTION_GET(NUMBER,file_quantity)

POOL_DEF(file_pool, FILE, FILE_QUANTITY);

FILE stdin_struct = {
	.fd = 0
};
FILE stdout_struct = {
	.fd = 1
};

FILE stderr_struct = {
	.fd = 1
};

FILE *stdin = &stdin_struct;
FILE *stdout = &stdout_struct;
FILE *stderr = &stderr_struct;

FILE *fopen(const char *path, const char *mode) {
	int fd;
	FILE *file = NULL;

	fd = open(path, 0);

	if (fd > 0) {
		file = pool_alloc(&file_pool);
		file->fd = fd;
	}

	return file;

}

FILE *freopen(const char *path, const char *mode, FILE *file) {
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
	return write(file->fd, buf, size * count);
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	char *cbuff;
	uint addon = 0;

	if (NULL == file) {
		/*errno = EBADF;*/
		return -1;
	}

	if(file->has_ungetc) {
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
	int res = close(file->fd);
	pool_free(&file_pool, file);
	return res;
}

int fseek(FILE *file, long int offset, int origin) {
	return lseek(file->fd, offset, origin);
}

int fstat(FILE *file, void *buff) {
	return stat(file->fd, buff);
}

int fioctl(FILE *fp, int request, ...) {
	va_list args;
	va_start(args, request);
	return ioctl(fp->fd, request, args);
}

int ungetc(int ch, FILE *file) {
	file->ungetc = (char) ch;
	file->has_ungetc = 1;
	return ch;
}
