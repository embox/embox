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
#include <sys/stat.h>
#include <string.h>
#include <grp.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif

#define EOVERFLOW 1
#define EMBOX
#define HAVE_STRCASECMP

#define MAP_PRIVATE   0x01
#define MAP_SHARED    0x00
#define PROT_READ     0x10
#define PROT_WRITE    0x20
#define _SC_PAGESIZE 0
#define O_ACCMODE       00000003
#define FD_CLOEXEC	(printf(">>> FC_CLOEXEC\n"),0)
#define OPEN_MAX      64

typedef __u16 u_int16_t;
typedef __u32 u_int32_t;
typedef __u64 u_int64_t;
typedef unsigned int uint;

static inline unsigned long long int strtoull(const char *nptr, char **endptr,
                                       int base) {
	printf(">>> strtoull\n");
	return strtoul(nptr, endptr, base);
}

static inline int strcasecmp(const char *s1, const char *s2) {
	printf(">>> strcasecmp, s1 - %s, s2 - %s\n", s1, s2);
	return strcmp(s1, s2);
}

static inline int strncasecmp(const char *s1, const char *s2, size_t n) {
	printf(">>> strncasecmp, s1 - %s, s2 - %s, n - %d\n", s1, s2, n);
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

static inline dev_t makedev(int maj, int min) {
	printf(">>> makedev, maj - %d, min - %d\n", maj, min);
	return -1;
}

static inline int fchmod(int fd, mode_t mode) {
	printf(">>> fchmod, fd - %d, mode - %d\n", fd, mode);
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

/****************** Functions for mke2fs *******************/

static inline FILE *popen(const char *command, const char *type) {
	printf(">>> popen, command - %s, type - %s\n", command, type);
	return NULL;
}

static inline int pclose(FILE *stream) {
	printf(">>> pclose\n");
	return -1;
}

static inline void sync(void) {
	printf(">>> sync\n");
}

static inline void setbuf(FILE *stream, char *buf) {
	printf(">>> setbuf, stream->fd - %d, buf - %p\n", stream->fd, buf);
}

static inline char *basename(char *path) {
	printf(">>> basename, path - %s\n", path);
	return NULL;
}

/****************** Functions for e2fsck *******************/

#define SA_SIGINFO 0x00000004
#define SI_USER         0

struct sigaction;

static inline int sigaction(int sig, const struct sigaction *act,
		struct sigaction *oact) {
	printf(">>> sigaction, sig - %d\n", sig);
	return -1;
}

static inline int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
	printf(">>> setvbuf, stream->fd - %d\n", stream->fd);
	return -1;
}

static inline void *sbrk(intptr_t increment) {
	printf(">>> sbrk, increment - %d\n", increment);
	return NULL;
}

static inline void tzset (void) {
	printf(">>> tzset\n");
}

static inline struct tm *localtime_r(const time_t *timep, struct tm *result) {
	printf(">>> localtime_r\n");
	return NULL;
}

static inline struct tm *gmtime_r(const time_t *timep, struct tm *result) {
	printf(">>> gmtime_r\n");
	return NULL;
}

static inline int daemon(int nochdir, int noclose) {
	printf(">>> daemon, nochdir - %d, noclose - %d\n", nochdir, noclose);
	return -1;
}

#endif /* E2FSPROGS_EMBOX_COMPAT_H_ */
