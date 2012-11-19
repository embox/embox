/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.10.2012
 */

#ifndef SRC_LIB_STDIO_FILE_POSIX_OPEN_H_
#define SRC_LIB_STDIO_FILE_POSIX_OPEN_H_

#include <fcntl.h>
#include <string.h>

static inline int __libc_open(const char *path, const char *mode,
		struct file_struct_int *file) {
	if ('r' == *mode) {
		file->fd = open(path, O_RDONLY);
	} else if ('w' == *mode) {
		file->fd = open(path, O_WRONLY);
	} else if ('a' == *mode) {
			file->fd = open(path, O_APPEND);
	} else {
		file->fd = open(path, 0);
	}
	return file->fd;
}

#endif /* SRC_LIB_STDIO_FILE_POSIX_OPEN_H_ */
