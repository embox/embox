/*
 * pthread.h
 *
 *  Created on: 25 mars 2013
 *      Author: fsulima
 */

#ifndef EMBOX_0MQ_PTHREAD_H_
#define EMBOX_0MQ_PTHREAD_H_

typedef int pthread_t;

typedef int pthread_mutex_t;
typedef int pthread_mutexattr_t;

typedef int pthread_attr_t;

static inline int pthread_mutex_init (pthread_mutex_t *__mutex,
                               __const pthread_mutexattr_t *__mutexattr) {
	DPRINT();
	return -1;
}

static inline int pthread_mutex_destroy (pthread_mutex_t *__mutex) {
	DPRINT();
	return -1;
}

static inline int pthread_mutex_lock (pthread_mutex_t *__mutex){
	DPRINT();
	return -1;
}
static inline int pthread_mutex_unlock (pthread_mutex_t *__mutex){
	DPRINT();
	return -1;
}

#define SIG_BLOCK 1

typedef int sigset_t;

static inline
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset) {
	DPRINT();
	return -1;
}

static inline
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg) {
	DPRINT();
	return EPERM;
}

static inline
int pthread_join(pthread_t thread, void **retval) {
	DPRINT();
	return EINVAL;
}

#endif /* EMBOX_0MQ_PTHREAD_H_ */
