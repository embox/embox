#ifndef TAR_EMBOX_COMPAT_H_
#define TAR_EMBOX_COMPAT_H_

#include <stddef.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>

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

#endif /* TAR_EMBOX_COMPAT_H_ */
