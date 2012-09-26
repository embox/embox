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


#ifdef __FILE_QUANTITY
  #define FILE_QUANTITY __FILE_QUANTITY
#else
  #define FILE_QUANTITY OPTION_GET(NUMBER,file_quantity)
#endif

POOL_DEF(file_pool, FILE, FILE_QUANTITY);

FILE stdin_struct = {
	.file_int = INIT_STDIN
};
FILE stdout_struct = {
	.file_int = INIT_STDOUT
};

FILE stderr_struct = {
	.file_int = INIT_STDERR
};

FILE *stdin = &stdin_struct;
FILE *stdout = &stdout_struct;
FILE *stderr = &stderr_struct;

FILE *fopen(const char *path, const char *mode) {
	FILE *file = pool_alloc(&file_pool);
	if (lopen(path, mode, (struct file_struct_int *) file) < 0) {
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
	return lwrite(buf, size, count, (struct file_struct_int *) file);
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

	return (addon + lread(buf, size, count, (struct file_struct_int *) file));
}


int fclose(FILE *file) {
	int res = lclose((struct file_struct_int *) file);
	pool_free(&file_pool, file);
	return res;
}

int fseek(FILE *file, long int offset, int origin) {
	return llseek((struct file_struct_int *) file, offset, origin);
}

int fioctl(FILE *fp, int request, ...) {
	va_list args;
	va_start(args, request);
	return lioctl((struct file_struct_int *) fp, request, args);
}

int ungetc(int ch, FILE *file) {
	file->ungetc = (char) ch;
	file->has_ungetc = 1;
	return ch;
}

