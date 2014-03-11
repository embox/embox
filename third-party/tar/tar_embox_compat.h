#ifndef TAR_EMBOX_COMPAT_H_
#define TAR_EMBOX_COMPAT_H_

#include <stddef.h>
#include <sys/types.h>
#include <errno.h>

extern ssize_t readlink(const char *path, char *buf,
		       size_t bufsize);
//static inline ssize_t readlink(const char *path, char *buf,
//		       size_t bufsize) {
//	(void)path; (void)buf; (void)bufsize;
//	return SET_ERRNO(ENOSYS);
//}

extern ssize_t readlinkat(int fd, const char *path,
		       char *buf, size_t bufsize);
//static inline ssize_t readlinkat(int fd, const char *path,
//		       char *buf, size_t bufsize) {
//	(void)fd; (void)path; (void)buf; (void)bufsize;
//	return SET_ERRNO(ENOSYS);
//}

#endif /* TAR_EMBOX_COMPAT_H_ */
