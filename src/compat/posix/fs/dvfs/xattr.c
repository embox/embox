/**
 * @file
 * @brief
 *
 * @author  Denis Deryugin
 * @date    3 Apr 2015
 */

#include <defines/size_t.h>

int getxattr(const char *path, const char *name, char *value, size_t size) {
	return 0;
}

int setxattr(const char *path, const char *name, const char *value, size_t size,
	       	int flags) {
	return 0;
}

int listxattr(const char *path, char *list, size_t size) {
	return 0;
}

int fsetxattr(int fd, const char *name, const char *value, size_t size, int flags) {
	return 0;
}

int fgetxattr(int fd, const char *name, void *value, size_t size) {
	return 0;
}

int flistxattr(int fd, char *list, size_t size) {
	return 0;
}

