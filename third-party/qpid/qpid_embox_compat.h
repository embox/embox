/*
 * ntfs-3g_embox_compat.h
 *
 *  Created on: 02 juillet 2013
 *      Author: fsulima
 */

#ifndef QPID_EMBOX_COMPAT_H_
#define QPID_EMBOX_COMPAT_H_


#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif

#include <stdio.h>
#include <string.h>

#if 1
#define DPRINT() printf(">>> gcc CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

extern
int toupper(int c);

#include <pthread.h>

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
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
extern
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);

extern "C"
int alphasort(const struct dirent **a, const struct dirent **b);
extern "C"
int scandir(const char *dirp, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compar)(const struct dirent **, const struct dirent **));

#define O_NOFOLLOW 0
#define F_TLOCK 1
#define F_ULOCK 2
extern
int lockf(int fd, int cmd, off_t len);

#ifdef __cplusplus

#endif // __cplusplus

#endif /* QPID_EMBOX_COMPAT_H_ */
