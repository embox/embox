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

#include <fs/core.h>


#ifdef __FILE_QUANTITY
  #define FILE_QUANTITY __FILE_QUANTITY
#else
  #define FILE_QUANTITY OPTION_GET(NUMBER,file_quantity)
#endif

POOL_DEF(file_pool, FILE, FILE_QUANTITY);

FILE *fopen(const char *path, const char *mode) {
	FILE *file = pool_alloc(&file_pool);
	file->file_int = kopen(path, mode);
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
	return kwrite(buf, size, count, file->file_int);
}

size_t fread(void *buf, size_t size, size_t count, FILE *file) {
	char *cbuff;
	uint addon = 0;

	if (NULL == file) {
		/*errno = EBADF;*/
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

	return (addon + kread(buf, size, count, file->file_int));
}

int fclose(FILE *file) {
	int res = kclose(file->file_int);
	pool_free(&file_pool, file);
	return res;
}

int fseek(FILE *file, long int offset, int origin) {
	return kseek(file->file_int, offset, origin);
}

int fstat(FILE *file, void *buff) {
	return kstat(file->file_int, buff);
}

int fioctl(FILE *fp, int request, ...) {
	va_list args;
	va_start(args, request);
	return kioctl(fp->file_int, request, args);
}

int ungetc(int ch, FILE *file) {
	file->ungetc = (char) ch;
	file->has_ungetc = 1;
	return ch;
}
