/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.07.2012
 */

#include <stdio.h>

extern ssize_t write(int fd, const void *buf, size_t nbyte);
extern ssize_t read(int fd, void *buf, size_t nbyte);

#define MAGIC_VAL 0xbadb0100

FILE *stdin = (FILE *) (MAGIC_VAL | 0x00);
FILE *stdout = (FILE *) (MAGIC_VAL | 0x01);
FILE *stderr = (FILE *) (MAGIC_VAL | 0x02);

FILE *fopen(const char *path, const char *mode) {
	return NULL;
}

FILE *freopen(const char *path, const char *mode, FILE *file) {
	return NULL;
}

int feof(FILE *file) {
	return 0;
}

int ferror(FILE *file) {
	return 0;
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	int fd;

	if (file != stdout && file != stderr) {
		return 0;
	}

	fd = (file == stdout ? 1 : 2);
	return write(fd, buf, size * count);
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {

	if (file != stdin) {
		return 0;
	}

	return read(0, buf, size * count);
}


int fclose(FILE *file) {
	return 0;
}

int fseek(FILE *file, long int offset, int origin) {
	return 0;
}

int fioctl(FILE *fp, int request, ...) {
	return 0;
}

int ungetc(int ch, FILE *file) {
	return 0;
}
