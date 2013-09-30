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
extern int   pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *, const struct timespec *);
extern int   pthread_key_create(pthread_key_t *, void (*)(void *));
extern int   pthread_key_delete(pthread_key_t);

#endif /* NTFS_EMBOX_COMPAT_H_ */
