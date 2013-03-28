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
//#include <pthread.h>
#include <errno.h>

#ifdef __cplusplus

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

#define _SC_PAGESIZE 0
	inline long sysconf(int name) {
		printf(">>> sysconf(%d)\n", name);
		switch (name) {
		case _SC_PAGESIZE: return 4096;
		default: break;
		}
		return -1;
	}

//#define MAP_SHARED    0x00
#define MAP_PRIVATE   0x01
#define PROT_READ     0x10
//#define PROT_WRITE    0x20
//#define MAP_FAILED    (-1)
#include <errno.h>
	static inline void  *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
		// ToDo: implement for InitFS files
		(void)addr;
		(void)len;
		(void)prot;
		(void)flags;
		(void)off;
		printf(">>> mmap(%i)\n",fd);
		errno = EPERM;
		return NULL;
	}
	static inline int munmap(void *addr, size_t size) {
		(void)size;
		printf(">>> munmap(%p)\n",addr);
		errno = EPERM;
		return -1;
	}

	static inline int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
		DPRINT();
		return -1;
	}

//	extern int ecvt_r(double number, int ndigits, int *decpt,
//	           int *sign, char *buf, size_t len);
//
//	extern int fcvt_r(double number, int ndigits, int *decpt,
//	           int *sign, char *buf, size_t len);
}

static inline
void *pthread_getspecific(struct pthread_key *) {
	DPRINT();
	return NULL;
}
static inline
int   pthread_setspecific(struct pthread_key *, const void *) {
	DPRINT();
	return ENOSYS;
}
static inline
int   pthread_key_create(struct pthread_key **, void (*)(void *)) {
	DPRINT();
	return ENOSYS;
}


#endif // __cplusplus

#endif /* STLPORT_EMBOX_COMPAT_H_ */
