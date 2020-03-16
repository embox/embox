/*
 * @file
 * @brief Get file type by it's header
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-12-10
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <fs/file_format.h>

static void print_usage(void) {
	printf("Usage: file [FILE PATH]\n");
}

int main(int argc, char **argv) {
	FILE *fd;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	if (NULL == (fd = fopen(argv[argc - 1], "r"))) {
		printf("Can't open file %s\n", argv[argc - 1]);
		return 0;
	}

	printf("%s: ", argv[argc - 1]);

	switch (libc_get_file_format(fd)) {
	case TEXT_FILE:
		printf("Text file\n");
		break;
	case RIFF_FILE:
		printf("RIFF audio file\n");
		break;
	case PNG_FILE:
		printf("PNG file\n");
		break;
	case GIF_FILE:
		printf("GIF file\n");
		break;
	case ELF_FILE:
		printf("ELF file\n");
		break;
	case XML_FILE:
		printf("XML file\n");
		break;
	default:
		printf("Unknown file format\n");
		break;
	}

	return 0;
}
