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
#include <time.h>

#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif

#define EMBOX

#include <sys/mman.h>

#define O_ACCMODE       00000003
//#define FD_CLOEXEC	(printf(">>> FC_CLOEXEC\n"),0)
#define OPEN_MAX      64

#include <stdint.h>
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;
typedef unsigned int uint;



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

static inline void setbuf(FILE *stream, char *buf) {
	printf(">>> setbuf, stream->fd - %d, buf - %p\n", stream->fd, buf);
}


/****************** Functions for e2fsck *******************/

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

static inline void sync(void) {
	printf("sync\n");
}

static inline int daemon(int nochdir, int noclose) {
	printf(">>> daemon, nochdir - %d, noclose - %d\n", nochdir, noclose);
	return -1;
}

static inline char *getwd(char *buf) {
	printf("getwd, buf - %p\n", buf);
	return NULL;
}

#endif /* E2FSPROGS_EMBOX_COMPAT_H_ */
