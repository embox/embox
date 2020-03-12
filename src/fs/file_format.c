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
	uint8_t elf_signature[4] = {0x7f, 0x45, 0x4c, 0x46};
	/* signature of XML files */
	uint8_t xml_signature[21] = {0x3c, 0x3f, 0x78, 0x6d, 0x6c, 0x20, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x3d, 0x22, 0x31, 0x2e, 0x30, 0x22, 0x3f, 0x3e};
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
	} else if (!memcmp(file, xml_signature, sizeof(xml_signature))) {
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
