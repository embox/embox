/**
 * @file
 * @brief Detect file format by header/etc
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-12-10
 */

#define FILE_HEADER_LEN 64

#include <stdio.h>
#include <stdint.h>

enum fformat {
	ERR_FILE = -1,
	TEXT_FILE,
};

extern enum fformat raw_get_file_format(uint8_t *file);
extern enum fformat libc_get_file_format(FILE *file);
extern enum fformat posix_get_file_format(int fd);
