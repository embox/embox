/**
 * @file
 * @brief Detect file format by header/etc
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-12-10
 */

#include <fs/file_format.h>

#include <string.h>
#include <unistd.h>

enum fformat raw_get_file_format(uint8_t *file) {
	/* Is it a RIFF file? */
	if (!memcmp(file, "RIFF", 4)) {
		/* TODO check file consistency */
		return RIFF_FILE;
	}

	return TEXT_FILE; /* Regular text file */
}

enum fformat posix_get_file_format(int fd) {
	uint8_t buf[FILE_HEADER_LEN];
	read(fd, buf, FILE_HEADER_LEN);
	return raw_get_file_format(buf);
}

enum fformat libc_get_file_format(FILE *file) {
	uint8_t buf[FILE_HEADER_LEN];
	fread(buf, 1, FILE_HEADER_LEN, file);
	return raw_get_file_format(buf);
}
