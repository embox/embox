/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.07.2012
 */

#include <stdio.h>
#include <prom/diag.h>

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
	char *cbuf = (char *) buf;
	char *cbufe = (char *) buf + size * count;

	if (file != stdout && file != stderr) {
		return 0;
	}

	while (cbuf != cbufe) {
		diag_putc(*cbuf);
		cbuf++;
	}
	return size * count;
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	char *cbuf = (char *) buf;
	char *cbufe = (char *) buf + size * count;

	if (file != stdin) {
		return 0;
	}

	while (cbuf != cbufe) {
		*cbuf = diag_getc();
		cbuf++;
	}
	return 0;
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
