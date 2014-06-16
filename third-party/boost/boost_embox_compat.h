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

#if 1
#define DPRINT() printf(">>> boost CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

#include <stdio.h>

//This can be used to derive sysconf from STLport
//using std::sysconf;


#include <pthread.h>

/* Structure describing CPU time used by a process and its children.  */

namespace std {
	extern size_t strxfrm(char *dest, const char *src, size_t n);
	extern size_t wcslen(const wchar_t *s);
}

extern int symlink(const char *oldpath, const char *newpath);
//extern int link(const char *oldpath, const char *newpath);
//extern ssize_t readlink(const char *path, char *buf, size_t bufsiz);
#define _PC_NAME_MAX 0
extern long pathconf(char *path, int name);

#include <utime.h>
extern int utime(const char *filename, const struct utimbuf *times);

int strerror_r(int errnum, char *buf, size_t buflen);

#include <time.h>
#include <pthread.h>


extern
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
extern
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);



extern
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
extern
int pthread_rwlock_init(pthread_rwlock_t * rwlock,
	const pthread_rwlockattr_t * attr);
extern
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
extern
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
extern
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);


extern
int swprintf(wchar_t *wcs, size_t maxlen,
	const wchar_t *format, ...);
extern
int vswprintf(wchar_t *wcs, size_t maxlen,
	const wchar_t *format, va_list args);

/* Length of interface name.  */
#define IF_NAMESIZE	16

#include <netinet/in.h>

struct ip_mreq  {
	struct in_addr imr_multiaddr;	/* IP multicast address of group */
	struct in_addr imr_interface;	/* local IP address of interface */
};

struct ipv6_mreq
  {
    /* IPv6 multicast address of group */
    struct in6_addr ipv6mr_multiaddr;
    /* local interface */
    unsigned int ipv6mr_interface;
  };


struct sockaddr_un {
    unsigned short sun_family;  /* AF_UNIX */
    char sun_path[90];
};

//FIXME
/* Maximum queue length specifiable by listen.  */
#define SOMAXCONN	4


/*
#define EAI_AGAIN	2
#define EAI_BADFLAGS	3
#define EAI_FAIL	4
#define EAI_FAMILY	5
#define EAI_MEMORY	6
#define EAI_NONAME	7
#define EAI_SERVICE	9
#define EAI_SOCKTYPE	10

#define AI_CANONNAME	0x0004
#define AI_NUMERICHOST	0x0008
#define AI_PASSIVE	0x0020

#define NI_MAXHOST	1025
#define NI_MAXSERV	32

#define NI_DGRAM 1
#define NI_NUMERICSERV 2

#define AI_ADDRCONFIG	0x0001
#define AI_PASSIVE	0x0020
*/

#include <signal.h>

#define SIG_BLOCK 1
#define SIG_SETMASK 2


extern
int socketpair(int domain, int type, int protocol, int sv[2]);
extern
int sockatmark(int sockfd);

extern
char *if_indextoname(unsigned ifindex, char *ifname);
extern
unsigned if_nametoindex(const char *ifname);

#endif /* BOOST_EMBOX_COMPAT_H_ */
