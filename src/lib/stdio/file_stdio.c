/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.09.2012
 */

#include <stdio.h>
#include <fs/fs_drv.h>
#include <fs/file_desc.h>
#include <prom/diag.h>

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
	int cnt = size * count;
	char *cbuf = (char *) buf;
	while (cnt--) {
		*cbuf++ = diag_getc();
	}
	return size * count;
}

static size_t stdio_write(const void *buf, size_t size, size_t count,
		void *file) {
	int cnt = size * count;
	char *cbuf = (char *) buf;
	while (cnt--) {
		diag_putc(*cbuf++);
	}
	return size * count;
}

static int stdio_ioctl(void *file, int request, va_list args) {
	return 0;
}

#define INIT_STDIO_FILE_DESC(_nm,_read_fn,_write_fn) \
	static file_operations_t _nm ## _fop = { \
	       .fopen = stdio_open, \
	       .fclose = stdio_close, \
	       .fread = _read_fn, \
	       .fwrite = _write_fn, \
	       .fseek =  NULL, \
	       .ioctl = stdio_ioctl, \
	       .fstat = NULL \
	}; \
	struct file_desc _nm ## _struct = { \
		.ops = & _nm ## _fop, \
	}; \


INIT_STDIO_FILE_DESC(stdin_int, stdio_read, NULL);
INIT_STDIO_FILE_DESC(stdout_int, NULL, stdio_write);
INIT_STDIO_FILE_DESC(stderr_int, NULL, stdio_write);
