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

static inline int __libc_open(const char *path, const char *mode) {
	int fd = open(path, 0);
	return fd;
}

#endif /* SRC_LIB_STDIO_FILE_POSIX_OPEN_H_ */
