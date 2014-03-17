#ifndef TAR_EMBOX_COMPAT_H_
#define TAR_EMBOX_COMPAT_H_

#include <stddef.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <utime.h>

extern ssize_t readlink(const char *path, char *buf,
		       size_t bufsize);

extern ssize_t readlinkat(int fd, const char *path,
		       char *buf, size_t bufsize);

static inline int WIFEXITED(int status) {
	printf(">>> %s %d\n", __func__, status);
	return 0;
}

static inline void flockfile(FILE *file) {
	printf(">>> %s %p\n", __func__, file);
}

static inline int ftrylockfile(FILE *file) {
	printf(">>> %s %p\n", __func__, file);
	return 0;
}

static inline void funlockfile(FILE *file) {
	printf(">>> %s %p\n", __func__, file);
}

static inline void rewinddir(DIR *dirp) {
	printf(">>> %s %p\n", __func__, dirp);
}

static inline int utime(const char *path,
		const struct utimbuf *times) {
	printf(">>> %s %s %p\n", __func__, path, times);
	return SET_ERRNO(ENOSYS);
}

static inline int execl(const char *path, const char *arg0,
		... /*, (char *)0 */) {
	printf(">>> %s %s %s\n", __func__, path, arg0);
	return SET_ERRNO(ENOSYS);
}

static inline int execlp(const char *file, const char *arg0,
		... /*, (char *)0 */) {
	printf(">>> %s %s %s\n", __func__, file, arg0);
	return SET_ERRNO(ENOSYS);
}

static inline int execvp(const char *file, char *const argv[]) {
	printf(">>> %s %s %s\n", __func__, file, argv[0]);
	return SET_ERRNO(ENOSYS);
}

static inline int getdtablesize(void) {
	printf(">>> %s\n", __func__);
	return 10;
}

#endif /* TAR_EMBOX_COMPAT_H_ */
