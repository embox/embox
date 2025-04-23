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

#define OPEN_MAX      64

#include <stdint.h>
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;
typedef unsigned int uint;

static inline int fchmod(int fd, mode_t mode) {
	printf(">>> fchmod, fd - %d, mode - %d\n", fd, mode);
	return -1;
}


/****************** Functions for e2fsck *******************/

#define SI_USER         0

struct sigaction;

static inline int sigaction(int sig, const struct sigaction *act,
		struct sigaction *oact) {
	printf(">>> sigaction, sig - %d\n", sig);
	return -1;
}

static inline void *sbrk(intptr_t increment) {
	printf(">>> sbrk, increment - %d\n", increment);
	return NULL;
}

static inline void tzset (void) {
	printf(">>> tzset\n");
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
