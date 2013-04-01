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

#define SQLITE_HOMEGROWN_RECURSIVE_MUTEX

#if 1
#define DPRINT() printf(">>> sqlite3 CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>

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


#endif /* SQLITE_EMBOX_COMPAT_H_ */
