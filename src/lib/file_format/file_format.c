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
	/* signature of PNG files */
	uint8_t png_signature[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
	/* signature of ELF files */
	uint8_t elf_signature[4] = {0x7f, 'E', 'L', 'F'};
	/* Is it a RIFF file? */
	if (!memcmp(file, "RIFF", 4)) {
		/* TODO check file consistency */
		return RIFF_FILE;
	/* Is it a BMP file? */
	} else if (!memcmp(file, "BM", 2)) {
		return BMP_FILE;
	} else if (!memcmp(file, png_signature, sizeof(png_signature))) {
		return PNG_FILE;
	} else if (!memcmp(file, "GIF", 3)) {
		return GIF_FILE;
	} else if (!memcmp(file, elf_signature, sizeof(elf_signature))) {
		return ELF_FILE;
	} else if (!memcmp(file, "<?xml", 5)) {
		return XML_FILE;
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
