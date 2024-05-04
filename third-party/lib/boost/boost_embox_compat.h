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

extern int symlink(const char *oldpath, const char *newpath);

#define _PC_NAME_MAX 0
extern long pathconf(char *path, int name);

extern int strerror_r(int errnum, char *buf, size_t buflen);

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
