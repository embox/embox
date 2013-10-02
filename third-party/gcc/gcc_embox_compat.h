/*
 * ntfs-3g_embox_compat.h
 *
 *  Created on: 02 juillet 2013
 *      Author: fsulima
 */

#ifndef NTFS_EMBOX_COMPAT_H_
#define NTFS_EMBOX_COMPAT_H_


#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif

#include <pthread.h>

#define PTHREAD_ONCE_INIT {}
extern int   pthread_once(pthread_once_t *, void (*)(void));
extern void *pthread_getspecific(pthread_key_t);
extern int   pthread_setspecific(pthread_key_t, const void *);
extern int   pthread_cancel(pthread_t);

extern int   pthread_key_create(pthread_key_t *, void (*)(void *));
extern int   pthread_key_delete(pthread_key_t);

#include <stdio.h>
#include <string.h>

#if 1
#define DPRINT() printf(">>> gcc CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

#ifdef __cplusplus

static inline int atexit(void (*function)(void)) {
	printf(">>> atexit(%p)\n",function);
}

static inline int strcoll(const char *s1, const char *s2) {
	printf(">>> strcoll(%s,%s)\n", s1, s2);
	return strcmp(s1, s2);
}

static inline size_t strxfrm(char *dest, const char *src, size_t n) {
	printf(">>> strxfrm(...)\n");
	return 0;
}

static inline
void clearerr(FILE *stream) {
	DPRINT();
}

static inline void setbuf(FILE *stream, char *buf) {
	printf(">>> setbuf, stream->fd - %d, buf - %p\n", stream->fd, buf);
}

static inline int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
	printf(">>> setvbuf, stream->fd - %d\n", stream->fd);
	return -1;
}

extern FILE *tmpfile(void);
extern char *tmpnam(char *s);

#endif // __cplusplus

#endif /* NTFS_EMBOX_COMPAT_H_ */
