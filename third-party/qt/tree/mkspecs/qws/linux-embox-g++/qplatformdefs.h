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

#if defined(__linux__)
#undef __linux__
#endif

#if defined(__linux)
#undef __linux
#endif

#define __MAKEDEPEND__

#define QT_NO_SOCKET_H

#define FD_CLOEXEC	1
#define F_DUPFD		printf(">>> FD_DUPFD\n"),0

#define fflush(x) printf(">>> fflush(%d)\n",(int)x)
#define getenv(x) printf(">>> getenv(%s)\n",x),NULL
//#define putenv(x) printf(">>> putenv(%s)\n",x),-1
#define putenv(x) -1

#include <stdio.h>
#define execvp(f,a) printf(">>> execvp(%s,...)\n",f),-1

#define sysconf(x) printf(">>> sysconf(%s)\n",#x),-1

#include <time.h>

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

#include "qglobal.h"

#include "../../common/posix/qplatformdefs.h"
//#include "../../linux-g++/qplatformdefs.h"
