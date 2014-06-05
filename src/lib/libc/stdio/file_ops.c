/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include <unistd.h>


#define DEFAULT_MODE 0755

static FILE stdin_struct = {
	.fd = STDIN_FILENO,
};
static FILE stdout_struct = {
	.fd = STDOUT_FILENO,
};
static FILE stderr_struct = {
	.fd = STDERR_FILENO,
};

FILE *stdin = &stdin_struct;
FILE *stdout = &stdout_struct;
FILE *stderr = &stderr_struct;

int feof(FILE *file) {
	//TODO:
	return 0;
}

int ferror(FILE *file) {
	//TODO:
	return 0;
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	int ret;

	if (NULL == file) {
		return -1;
	}

	ret = write(file->fd, buf, size * count);
	return ret > 0 ? ret / size : 0;
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	char *cbuff;
	size_t cnt;

	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	cnt = 0;
	if (file->has_ungetc) {
		file->has_ungetc = 0;
		cbuff = buf;
		cbuff[0] = (char)file->ungetc;
		count --;
		buf = &cbuff[1];
		cnt++;
	}
	while (cnt != count * size) {
		int tmp = read(file->fd,  buf, size * count);
		if (tmp == 0) {
			break; /* errors */
		}
		cnt += tmp;
	}
	if (cnt % size) {
		/* try to revert some bytes */
//		fpos_t pos;
//
//		fgetpos(file, &pos);
//		pos -= cnt % size;
//		fsetpos(file, &pos);
		cnt -= (cnt % size);
	}

	return cnt / size;
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

int fileno(FILE *stream) {
	assert(stream != NULL);
	return stream->fd;
}

