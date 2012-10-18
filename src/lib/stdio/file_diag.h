/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.10.2012
 */


#ifndef SRC_LIB_STDIO_FILE_DIAG_H_
#define SRC_LIB_STDIO_FILE_DIAG_H_

#include <prom/diag.h>

#define __FILE_QUANTITY 16

struct file_struct_int {
	int fd;
};

#define INIT_STDIO(_n) { .fd = _n }
#define INIT_STDIN INIT_STDIO(0)
#define INIT_STDOUT INIT_STDIO(1)
#define INIT_STDERR INIT_STDIO(1)

static inline size_t __libc_write(const void *buf, size_t size, size_t count, struct file_struct_int *file) {
	diag_putc(*(const char *) buf);
	return 1;
}

static inline size_t __libc_read(void *buf, size_t size, size_t count, struct file_struct_int *file) {
	* (char *) buf = diag_getc();
	return 1;
}

static inline int __libc_close(struct file_struct_int *file) {
	return -1;
}

static inline int __libc_lseek(struct file_struct_int *file, long int offset, int origin) {
	return -1;
}

static inline int __libc_fstat(struct file_struct_int *file, void *buff) {
	return -1;
}

static inline int __libc_ioctl(struct file_struct_int *file, int request, va_list args) {
	return -1;
}

#endif /* SRC_LIB_STDIO_FILE_DIAG_H_ */

