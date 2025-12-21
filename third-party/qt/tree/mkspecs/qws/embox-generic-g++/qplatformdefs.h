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

///#define QT_NO_FSFILEENGINE

// TEMPORARYFILE requires FSFILEENGINE
// Moved to command line
//#define QT_NO_TEMPORARYFILE

// Moved to command line
//#define QT_NO_FILESYSTEMWATCHER

// Moved to command line
//#define QT_NO_PROCESSlong pathconf(char *path, int name);

// Moved to command line
//#define QT_NO_NETWORKINTERFACE

#define QT_NO_INOTIFY

#define QT_NO_CRASHHANDLER

#define QT_NO_LOCALSOCKET

#define QT_NO_LOCALSERVER

#include <stdio.h>

#include <time.h>

#include <fcntl.h>

#include <dirent.h>

#include <sys/select.h>

#include <pwd.h>

#include <grp.h>

#include <sys/ioctl.h>

#include <unistd.h>

#include <errno.h>

#include <pthread.h>

#if 1
#define DPRINT() printf(">>> QT CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

#define O_LARGEFILE 0

inline int symlink(const char *oldpath, const char *newpath) {
	(void)oldpath; (void)newpath;
	printf(">>> symlink(%s, %s)\n", oldpath, newpath);
	errno = EPERM;
	return -1;
}

// Either this or define __GLIBC__
#include <limits.h>

#include <locale.h>

#include <arpa/inet.h>

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


#define TCP_NODELAY 0

// this is for FILESYSTEMWATCHER
#define _PC_LINK_MAX         0
#define _PC_MAX_CANON        1
#define _PC_MAX_INPUT        2
#define _PC_NAME_MAX         3
#define _PC_PATH_MAX         4
#define _PC_PIPE_BUF         5
#define _PC_CHOWN_RESTRICTED 6
#define _PC_NO_TRUNC         7
#define _PC_VDISABLE         8

static inline long pathconf(char *path, int name) {
	switch (name) {
	case _PC_NAME_MAX:
		return NAME_MAX;
	case _PC_PATH_MAX:
		return PATH_MAX;
	default:
		printf(">>> pathconf(%s,%d)\n",path,name);
		return 32;
	}
}

#endif // __QEMBOX__

#include "qglobal.h"

#include "../../common/posix/qplatformdefs.h"
//#include "../../linux-g++/qplatformdefs.h"
