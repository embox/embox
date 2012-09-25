/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.09.2012
 */


#ifndef LIB_STDIO_FILE_POSIX_H_
#define LIB_STDIO_FILE_POSIX_H_

#include <stddef.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

struct file_struct_int {
	int fd;
};

#define INIT_STDIO(_n) { .fd = _n }
#define INIT_STDIN INIT_STDIO(0)
#define INIT_STDOUT INIT_STDIO(1)
#define INIT_STDERR INIT_STDIO(1)

static inline int lopen(const char *path, const char *mode, struct file_struct_int *file) {
	file->fd = open(path, 0);
	return file->fd;
}

static inline size_t lwrite(const void *buf, size_t size, size_t count, struct file_struct_int *file) {
	return write(file->fd, buf, size * count);
}

static inline size_t lread(void *buf, size_t size, size_t count, struct file_struct_int *file) {
	return read(file->fd, buf, size * count);
}

static inline int lclose(struct file_struct_int *file) {
	return close(file->fd);
}

static inline int llseek(struct file_struct_int *file, long int offset, int origin) {
	return lseek(file->fd, offset, origin);
}

static inline int lioctl(struct file_struct_int *file, int request, va_list args) {
	return ioctl(file->fd, request, args);
}

#endif /* LIB_STDIO_FILE_POSIX_H_ */
