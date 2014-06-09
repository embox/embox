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

#include <unistd.h>

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

