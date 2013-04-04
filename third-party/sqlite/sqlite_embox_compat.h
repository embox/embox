/*
 * sqlite_embox_compat.h
 *
 *  Created on: 25 mars 2013
 *      Author: fsulima
 */

#ifndef SQLITE_EMBOX_COMPAT_H_
#define SQLITE_EMBOX_COMPAT_H_

#ifdef HAVE_POSIX_FALLOCATE
#undef HAVE_POSIX_FALLOCATE
#endif

#ifdef HAVE_LOCALTIME_R
#undef HAVE_LOCALTIME_R
#endif

#ifdef HAVE_STRERROR_R
#undef HAVE_STRERROR_R
#endif

#ifdef HAVE_READLINE
#undef HAVE_READLINE
#endif

#define SQLITE_HOMEGROWN_RECURSIVE_MUTEX
#define SQLITE_4_BYTE_ALIGNED_MALLOC

#define SQLITE_PRIVATE

#ifdef SQLITE_OMIT_TRACE
#undef SQLITE_OMIT_TRACE
#endif

#define SQLITE_ENABLE_IOTRACE 1

#define SQLITE_OMIT_WAL

#define SQLITE_DISABLE_LFS

#define SQLITE_TEMP_STORE 3

#define SQLITE_OMIT_SHARED_CACHE

#define SQLITE_THREADSAFE 0

//#ifdef SQLITE_THREADSAFE
//#undef SQLITE_THREADSAFE
//#endif

#if 1
#define DPRINT() printf(">>> sqlite3 CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static inline
int fchown(int fd, uid_t owner, gid_t group) {
	DPRINT();
	SET_ERRNO(EPERM);
	return -1;
}

static inline
int utimes(const char *filename, const struct timeval times[2]) {
	DPRINT();
	SET_ERRNO(EPERM);
	return -1;
}

#define MAP_SHARED    0x00
//#define MAP_PRIVATE   0x01
#define PROT_READ     0x10
#define PROT_WRITE    0x20
#define MAP_FAILED    ((void*)(-1))
#include <errno.h>
static inline void  *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
		// ToDo: implement for InitFS files
		(void)addr;
		(void)len;
		(void)prot;
		(void)flags;
		(void)off;
		printf(">>> mmap(%i)\n",fd);
		errno = EPERM;
		return MAP_FAILED;
	}
static inline int munmap(void *addr, size_t size) {
		(void)size;
		printf(">>> munmap(%p)\n",addr);
		errno = EPERM;
		return -1;
	}


struct rusage {
    struct timeval ru_utime; /* user CPU time used */
    struct timeval ru_stime; /* system CPU time used */
};
#define RUSAGE_SELF 0
static inline
int getrusage(int who, struct rusage *usage) {
	DPRINT();
	memset(usage, 0, sizeof(*usage));
	SET_ERRNO(EPERM);
	return -1;
}

static inline
FILE *popen(const char *command, const char *type) {
	DPRINT();
	return NULL;
}

static inline
int pclose(FILE *stream) {
	DPRINT();
	return -1;
}

static inline
int fchmod(int fd, mode_t mode) {
	DPRINT();
	SET_ERRNO(EPERM);
	return -1;
}

#endif /* SQLITE_EMBOX_COMPAT_H_ */
