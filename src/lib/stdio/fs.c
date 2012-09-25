/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <errno.h>
#include <mem/misc/pool.h>

#include <framework/mod/options.h>
#include <module/embox/fs/file_desc.h>

#include <fs/core.h>

POOL_DEF(file_pool, FILE, OPTION_MODULE_GET(embox__fs__file_desc,NUMBER,fdesc_quantity));

static inline int lopen(const char *path, const char *mode, FILE *file) {
	file->_.desc = kopen(path, mode);
	return (file->_.desc == NULL ? -1 : 0);
}

static inline size_t lwrite(const void *buf, size_t size, size_t count, FILE *file) {
	return kwrite(buf, size, count, file->_.desc);
}

static inline size_t lread(void *buf, size_t size, size_t count, FILE *file) {
	return kread(buf, size, count, file->_.desc);
}

static inline int lclose(FILE *file) {
	return kclose(file->_.desc);
}

static inline int lseek(FILE *file, long int offset, int origin) {
	return kseek(file->_.desc, offset, origin);
}

static inline int lioctl(FILE *file, int request, va_list args) {
	return kioctl(file->_.desc, request, args);
}

FILE *fopen(const char *path, const char *mode) {
	FILE *file = pool_alloc(&file_pool);
	if (lopen(path, mode, file) < 0) {
		return NULL;
	}
	return file;

}

FILE *freopen(const char *path, const char *mode, FILE *file) {
	return NULL;
}

int feof(FILE *file) {
	//TODO:
	return 0;
}

int ferror(FILE *file) {
	//TODO:
	return 0;
}

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	return lwrite(buf, size, count, file);
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	char *cbuff;
	uint addon = 0;

	if (NULL == file) {
		errno = EBADF;
		return -1;
	}

	if(file->has_ungetc) {
		file->has_ungetc = 0;
		cbuff = buf;
		cbuff[0] = (char)file->ungetc;
		count --;
		buf = &cbuff[1];
		addon = 1;
	}

	return (addon + lread(buf, size, count, file));
}


int fclose(FILE *file) {
	int res = lclose(file);
	pool_free(&file_pool, file);
	return res;
}

int fseek(FILE *file, long int offset, int origin) {
	return lseek(file, offset, origin);
}

int fioctl(FILE *fp, int request, ...) {
	va_list args;
	va_start(args, request);
	return lioctl(fp, request, args);
}

int ungetc(int ch, FILE *file) {
	file->ungetc = (char) ch;
	file->has_ungetc = 1;
	return ch;
}

