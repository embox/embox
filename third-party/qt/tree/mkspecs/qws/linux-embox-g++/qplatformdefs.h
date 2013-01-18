/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the qmake spec of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef __QEMBOX__
#define __QEMBOX__

// May be bad idea: causes to include byteswap.h everywhere
//#define __GLIBC__

//#define _POSIX_THREAD_SAFE_FUNCTIONS

#define QT_NO_FSFILEENGINE

// TEMPORARYFILE requires FSFILEENGINE
// Moved to command line
//#define QT_NO_TEMPORARYFILE

// Moved to command line
//#define QT_NO_FILESYSTEMWATCHER

// Moved to command line
//#define QT_NO_PROCESS

// Moved to command line
//#define QT_NO_NETWORKINTERFACE

#define QT_NO_INOTIFY

#define QT_NO_CRASHHANDLER

#define QT_NO_LOCALSOCKET

#define QT_NO_LOCALSERVER



#define FD_CLOEXEC	1
#define F_DUPFD		printf(">>> FD_DUPFD\n"),0

#include <stdio.h>
#define execvp(f,a) printf(">>> execvp(%s,...)\n",f),-1

#define sysconf(x) printf(">>> sysconf(%s)\n",#x),-1

#include <time.h>

#include <fcntl.h>

#include <dirent.h>

#include <sys/select.h>

#include <pwd.h>

#include <grp.h>

#include <sys/ioctl.h>

#include <unistd.h>



#define putenv(x) -1




typedef int pthread_t;

typedef int pthread_mutex_t;
typedef int pthread_cond_t;

typedef int pthread_mutexattr_t;
typedef int pthread_condattr_t;
typedef int pthread_attr_t;

extern int pthread_mutex_init (pthread_mutex_t *__mutex,
                               __const pthread_mutexattr_t *__mutexattr);
extern int pthread_mutex_destroy (pthread_mutex_t *__mutex);

extern int pthread_cond_init (pthread_cond_t *__restrict __cond,
                              __const pthread_condattr_t *__restrict
                              __cond_attr);
extern int pthread_cond_destroy (pthread_cond_t *__cond);

extern int pthread_mutex_lock (pthread_mutex_t *__mutex);
extern int pthread_mutex_unlock (pthread_mutex_t *__mutex);

extern int pthread_cond_wait (pthread_cond_t *__restrict __cond,
                              pthread_mutex_t *__restrict __mutex);
extern int pthread_cond_signal (pthread_cond_t *__cond);
extern int pthread_cond_timedwait (pthread_cond_t *__restrict __cond,
                                   pthread_mutex_t *__restrict __mutex,
                                   __const struct timespec *__restrict
                                   __abstime);



typedef int pthread_once_t;
#define PTHREAD_ONCE_INIT 0

#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 0

#define PTHREAD_CREATE_DETACHED 0

#define PTHREAD_INHERIT_SCHED 0

typedef unsigned int pthread_key_t;

extern int pthread_once (pthread_once_t *__once_control,
                         void (*__init_routine) (void));

extern void *pthread_getspecific (pthread_key_t __key);
extern int pthread_setspecific (pthread_key_t __key,
                                __const void *__pointer);

extern int pthread_key_create (pthread_key_t *__key,
                               void (*__destr_function) (void *));
extern int pthread_key_delete (pthread_key_t __key);

extern pthread_t pthread_self (void);

extern int pthread_setcancelstate (int __state, int *__oldstate);

extern void pthread_cleanup_push(void (*routine)(void*), void *arg);

extern void  pthread_cleanup_pop(int);

extern void  pthread_testcancel(void);

extern int pthread_attr_init(pthread_attr_t *);

extern int pthread_attr_setdetachstate(pthread_attr_t *, int);

extern int pthread_create(pthread_t *, const pthread_attr_t *,
          void *(*)(void *), void *);

extern int pthread_attr_setinheritsched(pthread_attr_t *, int);

extern int pthread_attr_destroy(pthread_attr_t *);

extern int pthread_cancel(pthread_t);

extern int pthread_cond_broadcast(pthread_cond_t *);




struct tm * localtime ( const time_t * timer );

void tzset(void);
extern char *tzname[2];



long ftello(FILE *stream);
int fseeko(FILE *stream, off_t offset, int whence);

#define O_LARGEFILE 0



#define F_RDLCK 0
#define F_WRLCK 0
#define F_SETLKW 0
#define F_UNLCK 0
#define F_SETLK 0

struct flock {
  short  l_type;
  short  l_whence;
  off_t  l_start;
  off_t  l_len;
  pid_t  l_pid;
};

ssize_t readlink(const char *path, char *buf, size_t bufsiz);

int getpwuid_r(uid_t uid, struct passwd *pwd,
	       char *buf, size_t buflen, struct passwd **result);

int access(const char *pathname, int mode);
#define R_OK 0
#define W_OK 0
#define X_OK 0
#define F_OK 0
int rename(const char *oldpath, const char *newpath);
int symlink(const char *oldpath, const char *newpath);

int chdir(const char *path);

char *get_current_dir_name(void);



// Either this or define __GLIBC__
#define PATH_MAX 256
char *getcwd(char *buf, size_t size);




char *setlocale(int category, const char *locale);
#define LC_ALL 0
#define LC_CTYPE 0


#define RTLD_LAZY 0
#define RTLD_GLOBAL 0
#define RTLD_LOCAL 0




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
extern clock_t times (struct tms *__buffer);

typedef int sig_atomic_t;

// Bad thing to do
#define NSIG 0






#include <net/socket.h>
//#include <netinet/ip.h>
#include <arpa/inet.h>


struct hostent *gethostbyaddr(const void *addr,
			      socklen_t len, int type);

int gethostname(char *name, size_t len);


typedef __u32 u_int32_t;
typedef __u16 u_int16_t;

#ifndef __res_state_defined
# define __res_state_defined

typedef enum { res_goahead, res_nextns, res_modified, res_done, res_error }
	res_sendhookact;

typedef res_sendhookact (*res_send_qhook) (struct sockaddr_in * const *__ns,
					   const u_char **__query,
					   int *__querylen,
					   u_char *__ans,
					   int __anssiz,
					   int *__resplen);

typedef res_sendhookact (*res_send_rhook) (const struct sockaddr_in *__ns,
					   const u_char *__query,
					   int __querylen,
					   u_char *__ans,
					   int __anssiz,
					   int *__resplen);

/*
 * Global defines and variables for resolver stub.
 */
# define MAXNS			3	/* max # name servers we'll track */
# define MAXDFLSRCH		3	/* # default domain levels to try */
# define MAXDNSRCH		6	/* max # domains in search path */
# define LOCALDOMAINPARTS	2	/* min levels in name that is "local" */

# define RES_TIMEOUT		5	/* min. seconds between retries */
# define MAXRESOLVSORT		10	/* number of net to sort on */
# define RES_MAXNDOTS		15	/* should reflect bit field size */
# define RES_MAXRETRANS		30	/* only for resolv.conf/RES_OPTIONS */
# define RES_MAXRETRY		5	/* only for resolv.conf/RES_OPTIONS */
# define RES_DFLRETRY		2	/* Default #/tries. */
# define RES_MAXTIME		65535	/* Infinity, in milliseconds. */

struct __res_state {
	int	retrans;		/* retransmition time interval */
	int	retry;			/* number of times to retransmit */
	u_long	options;		/* option flags - see below. */
	int	nscount;		/* number of name servers */
	struct sockaddr_in
		nsaddr_list[MAXNS];	/* address of name server */
# define nsaddr	nsaddr_list[0]		/* for backward compatibility */
	u_short	id;			/* current message id */
	/* 2 byte hole here.  */
	char	*dnsrch[MAXDNSRCH+1];	/* components of domain to search */
	char	defdname[256];		/* default domain (deprecated) */
	u_long	pfcode;			/* RES_PRF_ flags - see below. */
	unsigned ndots:4;		/* threshold for initial abs. query */
	unsigned nsort:4;		/* number of elements in sort_list[] */
	unsigned ipv6_unavail:1;	/* connecting to IPv6 server failed */
	unsigned unused:23;
	struct {
		struct in_addr	addr;
		u_int32_t	mask;
	} sort_list[MAXRESOLVSORT];
	/* 4 byte hole here on 64-bit architectures.  */
	res_send_qhook qhook;		/* query hook */
	res_send_rhook rhook;		/* response hook */
	int	res_h_errno;		/* last one set for this context */
	int	_vcsock;		/* PRIVATE: for res_send VC i/o */
	u_int	_flags;			/* PRIVATE: see below */
	/* 4 byte hole here on 64-bit architectures.  */
	union {
		char	pad[52];	/* On an i386 this means 512b total. */
		struct {
			u_int16_t		nscount;
			u_int16_t		nsmap[MAXNS];
			int			nssocks[MAXNS];
			u_int16_t		nscount6;
			u_int16_t		nsinit;
			struct sockaddr_in6	*nsaddrs[MAXNS];
#ifdef _LIBC
			unsigned long long int	initstamp
			  __attribute__((packed));
#else
			unsigned int		_initstamp[2];
#endif
		} _ext;
	} _u;
};

#endif



#define IP_MULTICAST_TTL 0
#define IPV6_MULTICAST_LOOP 0
#define IP_MULTICAST_LOOP 0
#define FIONREAD 0



uid_t getuid(void);



#endif // __QEMBOX__

#include "qglobal.h"

#include "../../common/posix/qplatformdefs.h"
//#include "../../linux-g++/qplatformdefs.h"
