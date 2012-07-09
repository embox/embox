/**
 * @file
 * @brief defines stdio files (stdin, stdout, stderr)
 *
 * @author Anton Kozlov
 * @version
 * @date 14.05.2012
 */

#include <fs/fs_drv.h>
#include <fs/vfs.h>
#include <util/array.h>
#include <stdio.h>

#include <fs/file_desc.h>

static int _determ_flide(FILE *stream) {

	assert(stream == stdin || stream == stdout || stream == stderr);

	if ((int) stream == (int) stdin) {
		return 0;
	}

	if ((int) stream == (int) stdout) {
		return 1;
	}

	if ((int) stream == (int) stderr) {
		return 3;
	}

	return -1;
}

/*
 * file_operation
 */
static void *stdio_open(struct file_desc *desc, const char *mode) {
	return NULL;
}

static int stdio_close(struct file_desc *desc) {
	return -1;
}

static size_t stdio_read(void *buf, size_t size, size_t count, void *file) {
	int fd = _determ_flide((FILE *) file);
	return read(fd, buf, size * count);
}

static size_t stdio_write(const void *buf, size_t size, size_t count,
		void *file) {
	int fd = _determ_flide((FILE *) file);
	return write(fd, buf, size * count);
}

static int stdio_ioctl(void *file, int request, va_list args) {
	return 0;
}

static file_operations_t stdio_fop = {
       .fopen = stdio_open,
       .fclose = stdio_close,
       .fread = stdio_read,
       .fwrite = stdio_write,
       .fseek =  NULL,
       .ioctl = stdio_ioctl
};

#define INIT_STDIO_FILE_DESC(_nm) \
	struct file_desc _nm = { \
		.ops = &stdio_fop, \
	}

INIT_STDIO_FILE_DESC(_stdin);
INIT_STDIO_FILE_DESC(_stdout);
INIT_STDIO_FILE_DESC(_stderr);

FILE * stdin = (FILE *) &_stdin;
FILE * stdout = (FILE *) &_stdout;
FILE * stderr = (FILE *) &_stderr;
