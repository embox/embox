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

static inline
long sysconf(int name) {
#define _SC_CLK_TCK 1
	printf(">>> sysconf(%d)\n", name);
	switch (name) {
		case _SC_PAGESIZE: return 4096;
		case _SC_CLK_TCK: return 1000000;
		default: break;
	}
	return -1;
}

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
extern int link(const char *oldpath, const char *newpath);
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
int pthread_attr_init(pthread_attr_t *attr);
extern
int pthread_attr_destroy(pthread_attr_t *attr);
extern
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
extern
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);

#define PTHREAD_CREATE_DETACHED 0
extern
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
extern
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);

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

extern
int getpagesize(void);

#endif /* SAMBA_EMBOX_COMPAT_H_ */
