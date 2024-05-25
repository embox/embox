/*
 * ntfs-3g_embox_compat.h
 *
 *  Created on: 02 juillet 2013
 *      Author: fsulima
 */

#ifndef QPID_EMBOX_COMPAT_H_
#define QPID_EMBOX_COMPAT_H_


#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>

#if 1
#define DPRINT() printf(">>> qpid CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

#include <ctype.h>


#include <errno.h>

__BEGIN_DECLS

#include <pthread.h>

//#include <dirent.h>
static inline
int alphasort(const struct dirent **a, const struct dirent **b) {
	(void)a;
	(void)b;
	DPRINT();
	return 0;
}
static inline
int scandir(const char *dirp, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compar)(const struct dirent **, const struct dirent **)) {
	(void)dirp;
	(void)namelist;
	(void)filter;
	(void)compar;
	DPRINT();
	errno = ENOMEM;
	return -1;
}


#define F_TLOCK 0x01
#define F_ULOCK 0x02

static inline
int lockf(int fd, int cmd, off_t len) {
	(void)fd;
	(void)cmd;
	(void)len;
	DPRINT();
	return 0;
}

static inline pid_t setsid(void) {
	DPRINT();
	return -1;
}

static inline
pid_t getppid(void) {
	DPRINT();
	return 0;
}


static inline pid_t fork() {
	printf(">>> fork()\n");
	errno = ENOSYS;
	return -1;
}

#include <sys/mman.h>


#include <sys/socket.h>

__END_DECLS

#include <netinet/in.h>

#define TCP_NODELAY 0

#define NI_MAXHOST	1025
#define NI_MAXSERV	32

#include <arpa/inet.h>

static inline
char *strerror_r(int errnum, char *buf, size_t buflen) {
	(void)errnum;
	(void)buf;
	(void)buflen;
	DPRINT();
	return strerror(errnum);
}


/* not standard */
typedef unsigned int uint;


#include <signal.h>


// instead of gcc Thread-Local Storage
#define __thread

// some compilers defines __unix__ (i386 gcc), some __unix (arm-eabi gcc),
// qpid expects it to be __unix__ (in regex determinition),
// so definig one to another
#define __unix__ __unix

#endif /* QPID_EMBOX_COMPAT_H_ */
