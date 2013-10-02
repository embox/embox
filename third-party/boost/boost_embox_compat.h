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
using std::sysconf;
/*
static inline
long sysconf(int name) {
//#define _SC_CLK_TCK 1
	printf(">>> sysconf(%d)\n", name);
	switch (name) {
		case _SC_PAGESIZE: return 4096;
//		case _SC_CLK_TCK: return
		default: break;
	}
	return -1;
}
*/


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

extern
int getpagesize(void);

#endif /* SAMBA_EMBOX_COMPAT_H_ */
