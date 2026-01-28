#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void *__fini_array_start = (void *) 0xdeadbeef;
void *__fini_array_end = (void *) 0xdeadbeef;
void *__init_array_start = (void *) 0xdeadbeef;
void *__init_array_end = (void *) 0xdeadbeef;
void *__preinit_array_start = (void *) 0xdeadbeef;
void *__preinit_array_end = (void *) 0xdeadbeef;

//void S_WriteLinearBlastStereo16 (void) { }

int __isoc99_sscanf(const char *out, const char *format, ...) {
	va_list args;
	int rv;

	va_start(args, format);
	rv = sscanf(out, format, args);
	va_end (args);

	return rv;
}

int _setjmp(jmp_buf b) {
	return setjmp(b);
}

int __xstat(int ver, const char * path, struct stat * stat_buf) {
	return 0;
}

int __lxstat(int ver, const char * path, struct stat * stat_buf) {
	return 0;
}

int __fxstat(int ver, int fildes, struct stat * stat_buf) {
	return 0;
}

int * __errno_location(void) {
	return 0;
}

int ftello64 (void *s) {
	return 0;
}

int fseeko64(void *s, uint64_t o, int w) {
	return 0;
}

FILE *freopen64(const char *filename, const char *type, FILE *stream) {
	return 0;
}

FILE  *fopen64(char *p, char *m) {
	return 0;
}

int pkey_mprotect(void *addr, size_t len, int prot, int pkey) {
	return 0;
}

char * __xpg_basename(const char * path) {
	return 0;
}
