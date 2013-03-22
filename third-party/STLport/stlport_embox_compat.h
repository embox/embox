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
#define CHAR_MAX (127)
#define CHAR_MIN (-128)


#if 1
#define DPRINT() printf(">>> STLport CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

#include_next <stdio.h>
#include_next <time.h>

// it is not immediately apparent, but if we write "namespace std" it is changed to some other name
namespace std {
	static inline char *asctime(const struct tm *tm) {
		DPRINT();
		time_t t = mktime((struct tm *)tm);
		return ctime(&t);
	}
	static inline double difftime(time_t time1, time_t time0) {
		DPRINT();
		return (double)(time1 - time0);
	}
	static inline struct tm *localtime(const time_t *timep) {
		DPRINT();
		return gmtime(timep);
	}
	static inline char *getenv(const char *name) {
		DPRINT();
		return NULL;
	}
	static inline int mblen(const char *s, size_t n) {
		DPRINT();
		// FIXME:
		return 0;
	}
	static inline int mbtowc(wchar_t *pwc, const char *s, size_t n) {
		DPRINT();
		return -1;
	}
	static inline int system(const char *command) {
		DPRINT();
		return -1;
	}
	static inline int atexit(void (*function)(void)) {
		DPRINT();
		return -1;
	}
	static inline void exit(int status) {
		DPRINT();
		while(true) {;}
	}
	static inline size_t mbstowcs(wchar_t *dest, const char *src, size_t n) {
		DPRINT();
		return -1;
	}
}

#endif /* STLPORT_EMBOX_COMPAT_H_ */
