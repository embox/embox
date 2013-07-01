/**
 * @file
 * @brief Stubs for e2fsprogs
 *
 * @author  Alexander Kalmuk
 * @date    30.06.2013
 */

#ifndef E2FSPROGS_EMBOX_COMPAT_H_
#define E2FSPROGS_EMBOX_COMPAT_H_

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <grp.h>
#include <stdlib.h>

#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif

#define EOVERFLOW 1
#define EMBOX

#define MAP_PRIVATE   0x01
#define MAP_SHARED    0x00
#define PROT_READ     0x10
#define PROT_WRITE    0x20
#define _SC_PAGESIZE 0
#define O_ACCMODE       00000003
#define FD_CLOEXEC	(printf(">>> FC_CLOEXEC\n"),0)
#define OPEN_MAX      64

static inline unsigned long long int strtoull(const char *nptr, char **endptr,
                                       int base) {
	printf(">>> strtoull\n");
	return strtoul(nptr, endptr, base);
}

static inline int strcasecmp(const char *s1, const char *s2) {
	printf(">>> strcasecmp\n");
	return strcmp(s1, s2);
}

static inline int strncasecmp(const char *s1, const char *s2, size_t n) {
	printf(">>> strncasecmp\n");
	return strncmp(s1, s2, n);
}

static inline struct group *getgrgid(gid_t gid) {
	printf(">>> getgrgid. gid - %d\n", gid);
	return NULL;
}

static inline long sysconf(int name) {
	printf(">>> sysconf, name - %d\n", name);
	switch(name) {
	case _SC_PAGESIZE:
		return 4096;
	}
	return -1;
}

static inline int getpagesize(void) {
	printf(">>> getpagesize\n");
	return 4096;
}

static inline void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset) {
	printf(">>> mmap, addr - %p\n", addr);
	return 0;
}

static inline int munmap(void *addr, size_t length) {
	printf(">>> munmap, addr - %p\n", addr);
	return -1;
}

static inline ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
	printf(">>> pread, fd - %d, offset - %d\n", fd, offset);
	return -1;
}

static inline ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
	printf(">>> pwrite, fd - %d, offset - %d\n", fd, offset);
	return -1;
}

#endif /* E2FSPROGS_EMBOX_COMPAT_H_ */
