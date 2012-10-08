/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.10.2012
 */

#ifndef SRC_LIB_STDIO_FILE_POSIX_NOOPEN_H_
#define SRC_LIB_STDIO_FILE_POSIX_NOOPEN_H_

static inline int __libc_open(const char *path, const char *mode, struct file_struct_int *file) {
	return -1;
}

#endif /* SRC_LIB_STDIO_FILE_POSIX_NOOPEN_H_ */

