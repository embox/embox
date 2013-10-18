/*
 * samba_embox_compat.h
 *
 *  Created on: 21 mars 2013
 *      Author: fsulima
 */

#ifndef SAMBA_EMBOX_COMPAT_H_
#define SAMBA_EMBOX_COMPAT_H_

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
struct tms
{
  clock_t tms_utime;          /* User CPU time.  */
  clock_t tms_stime;          /* System CPU time.  */

  clock_t tms_cutime;         /* User CPU time of dead children.  */
  clock_t tms_cstime;         /* System CPU time of dead children.  */
};

/* Store the CPU time used by this process and all its
   dead children (and their dead children) in BUFFER.
   Return the elapsed real time, or (clock_t) -1 for errors.
   All times are in CLK_TCKths of a second.  */
static inline clock_t times (struct tms *__buffer) {
	//DPRINT();
	__buffer->tms_cstime = __buffer->tms_cutime = 0;
	__buffer->tms_stime = task_self()->per_cpu;
	__buffer->tms_utime = 0;

	return __buffer->tms_stime;
}


namespace std {
	extern size_t strxfrm(char *dest, const char *src, size_t n);
	extern size_t wcslen(const wchar_t *s);
}

extern int symlink(const char *oldpath, const char *newpath);
//extern int link(const char *oldpath, const char *newpath);
extern ssize_t readlink(const char *path, char *buf, size_t bufsiz);
#define _PC_NAME_MAX 0
extern long pathconf(char *path, int name);

#include <utime.h>
extern int utime(const char *filename, const struct utimbuf *times);

int strerror_r(int errnum, char *buf, size_t buflen);

#include <time.h>
struct tm *localtime_r(const time_t *timep, struct tm *result);

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


/* Structure large enough to hold any socket address (with the historical
   exception of AF_UNIX).  We reserve 128 bytes.  */
#define __ss_aligntype	unsigned long int
#define _SS_SIZE	128
#define _SS_PADSIZE	(_SS_SIZE - (2 * sizeof (__ss_aligntype)))

#define	__SOCKADDR_COMMON(sa_prefix) \
  sa_family_t sa_prefix##family

#define __SOCKADDR_COMMON_SIZE	(sizeof (unsigned short int))

struct sockaddr_storage
  {
    __SOCKADDR_COMMON (ss_);	/* Address family, etc.  */
    __ss_aligntype __ss_align;	/* Force desired alignment.  */
    char __ss_padding[_SS_PADSIZE];
  };


struct sockaddr_un {
    unsigned short sun_family;  /* AF_UNIX */
    char sun_path[108];
};

struct addrinfo
{
  int ai_flags;			/* Input flags.  */
  int ai_family;		/* Protocol family for socket.  */
  int ai_socktype;		/* Socket type.  */
  int ai_protocol;		/* Protocol for socket.  */
  socklen_t ai_addrlen;		/* Length of socket address.  */
  struct sockaddr *ai_addr;	/* Socket address for socket.  */
  char *ai_canonname;		/* Canonical name for service location.  */
  struct addrinfo *ai_next;	/* Pointer to next in list.  */
};

//FIXME
/* Maximum queue length specifiable by listen.  */
#define SOMAXCONN	4

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

static inline
int getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res) {
	DPRINT();
	return EAI_FAIL;
}

extern
void freeaddrinfo(struct addrinfo *res);

#define NI_DGRAM 1
#define NI_NUMERICSERV 2
extern
int getnameinfo(const struct sockaddr *sa, socklen_t salen,
                       char *host, size_t hostlen,
                       char *serv, size_t servlen, int flags);

#define AI_ADDRCONFIG	0x0001
#define AI_PASSIVE	0x0020


#define SIG_BLOCK 1
#define SIG_SETMASK 2

typedef int sigset_t;

static inline
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset) {
	DPRINT();
	return 0;
}

static inline
int sigfillset(sigset_t *set) {
	DPRINT();
	*set = -1;
	return 0;
}

extern
int socketpair(int domain, int type, int protocol, int sv[2]);
extern
int sockatmark(int sockfd);

extern
char *if_indextoname(unsigned ifindex, char *ifname);
extern
unsigned if_nametoindex(const char *ifname);

#endif /* SAMBA_EMBOX_COMPAT_H_ */
