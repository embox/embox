/*
 * stlport_embox_compat.h
 *
 *  Created on: 21 mars 2013
 *      Author: fsulima
 */

#ifndef STLPORT_EMBOX_COMPAT_H_
#define STLPORT_EMBOX_COMPAT_H_

#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif

// FIXME: for yet obscure reason despite limits.h is included, but these constants are not defined.
//         No time for it now, let's figure out why later.
#include <limits.h>
//#define CHAR_MAX (127)
//#define CHAR_MIN (-128)


#if 1
#define DPRINT() printf(">>> STLport CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

#include_next <stdio.h>
#include_next <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

#ifdef __cplusplus

// it is not immediately apparent, but if we write "namespace std" it is changed to some other name
namespace std {
#if 0
/* implemented now */
	static inline char *asctime(const struct tm *tm) {
		DPRINT();
		time_t t = mktime((struct tm *)tm);
		return ctime(&t);
	}
	static inline struct tm *localtime(const time_t *timep) {
		DPRINT();
		return gmtime(timep);
	}
	static inline int system(const char *command) {
		DPRINT();
		return -1;
	}
#endif
	static inline double difftime(time_t time1, time_t time0) {
		DPRINT();
		return (double)(time1 - time0);
	}
	static inline int mblen(const char *s, size_t n) {
		(void)s;
		(void)n;
		DPRINT();
		// FIXME:
		return 0;
	}
	static inline int mbtowc(wchar_t *pwc, const char *s, size_t n) {
		(void)pwc;
		(void)s;
		(void)n;
		DPRINT();
		return -1;
	}
	static inline int atexit(void (*function)(void)) {
		(void)function;
		DPRINT();
		return -1;
	}
	static inline size_t mbstowcs(wchar_t *dest, const char *src, size_t n) {
		(void)dest;
		(void)src;
		(void)n;
		DPRINT();
		return -1;
	}
	static inline int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
		(void)stream;
		(void)buf;
		(void)mode;
		(void)size;
		DPRINT();
		return -1;
	}

//	extern int ecvt_r(double number, int ndigits, int *decpt,
//	           int *sign, char *buf, size_t len);
//
//	extern int fcvt_r(double number, int ndigits, int *decpt,
//	           int *sign, char *buf, size_t len);
}


/* implemented now
 * extern int nanosleep(const struct timespec *req, struct timespec *rem);
 */

extern void clearerr(FILE *stream);

#endif // __cplusplus

#include <defines/wchar_t.h>

// moving this section to _embox.h
//#ifdef __WINT_TYPE__
//typedef __WINT_TYPE__ wint_t;
//#else //__WINT_TYPE__
//#error __WINT_TYPE__ is not defined
//#endif //__WINT_TYPE__

#ifdef __WCHAR_MIN__
#define WCHAR_MIN __WCHAR_MIN__
#endif

#ifdef __WCHAR_MAX__
#define WCHAR_MAX __WCHAR_MAX__
#endif

#endif /* STLPORT_EMBOX_COMPAT_H_ */
