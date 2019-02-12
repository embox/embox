/**
 * @file dlfcn.c
 * @brief Stubs for dynamic linking stuff
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 25.10.2018
 */

#include <dlfcn.h>

void *dlopen(const char *a, int b) {
	return 0;
}

void *dlsym(void *a, const char *b) {
	return 0;
}

int dlclose(void *a) {
	return 0;
}

char *dlerror(void) {
	return 0;
}
