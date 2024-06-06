/*
 * boost_embox_compat.h
 *
 *  Created on: 21 mars 2013
 *      Author: fsulima
 */

#ifndef BOOST_EMBOX_COMPAT_H_
#define BOOST_EMBOX_COMPAT_H_

#ifdef linux
#undef linux
#endif

#ifdef __linux
#undef __linux
#endif

#ifdef __linux__
#undef __linux__
#endif

#include <stdio.h>
#include <pthread.h>

/* Structure describing CPU time used by a process and its children.  */

namespace std {
	//extern size_t strxfrm(char *dest, const char *src, size_t n);
	//extern size_t wcslen(const wchar_t *s);
}

#define RLIM_INFINITY 0

static inline ssize_t readlink(const char *path, char *buf, size_t bufsiz) {
	printf("boost>>> %s\n", __func__);
	return -1;
}

static inline int symlink(const char *oldpath, const char *newpath) {
	printf("boost>>> %s\n", __func__);
	return -1;
}

static inline int utime(const char *filename, const struct utimbuf *times) {
	printf("boost>>> %s\n", __func__);
	return -1;
}

static inline long pathconf(char *path, int name) {
	printf("boost>>> %s\n", __func__);
	return -1;
}

#define _PC_NAME_MAX 0

#include <string.h>
static inline int strerror_r(int errnum, char *buf, size_t buflen) {
	// memcpy(buf, strerror(errnum), buflen);
	return 0;
}

static inline int statvfs(const char *path, struct statvfs *buf) {
	return -1;
}

static inline int link(const char *oldpath, const char *newpath) {
	printf("boost>>> %s\n", __func__);
	return -1;
}

#include <pthread.h>

extern
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);

extern
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);

/* Length of interface name.  */
#define IF_NAMESIZE	16

#include <netinet/in.h>

//FIXME
/* Maximum queue length specifiable by listen.  */
#define SOMAXCONN	4

extern
int sockatmark(int sockfd);

extern
char *if_indextoname(unsigned ifindex, char *ifname);

#endif /* BOOST_EMBOX_COMPAT_H_ */
