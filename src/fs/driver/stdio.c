/**
 * @file
 * @brief defines stdio files (stdin, stdout, stderr)
 *
 * @author Anton Kozlov
 * @version
 * @date 14.05.2012
 */

#include <fs.h>
#include <fs/vfs.h>
#include <util/array.h>
#include <stdio.h>

#include <fs/file_desc.h>

static int stdio_init(void * par) {
	return 0;
}

static int stdio_format(void *par) {
	return 0;
}

static int stdio_mount(void *par) {
	return 0;
}

static int stdio_create(void *params) {
	return 0;
}

static int stdio_delete(const char *fname) {
	return 0;
}

static int _determ_flide(FILE *stream) {

	if ((int) stream == (int) stdin) {
		return 0;
	} else if ((int) stream == (int) stdout) {
		return 1;
	} else if ((int) stream == (int) stderr) {
		return 3;
	}

	assert (0 == 1);

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

static fsop_desc_t stdio_fsop = { stdio_init, stdio_format, stdio_mount,
		stdio_create, stdio_delete};

static file_operations_t stdio_fop = {
       .fopen = stdio_open,
       .fclose = stdio_close,
       .fread = stdio_read,
       .fwrite = stdio_write,
       .fseek =  NULL,
       .ioctl = stdio_ioctl
};

static const fs_drv_t stdio_drv = {
	.name = "stdio",
	.file_op = &stdio_fop,
	.fsop = &stdio_fsop
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

/*DECLARE_FILE_SYSTEM_DRIVER(stdio_drv);*/
