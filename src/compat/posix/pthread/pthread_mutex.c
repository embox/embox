/**
 * @file
 *
 * @date Oct 15, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <pthread.h>

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	return ENOERR;
}

/*
int pthread_mutex_getprioceiling(const pthread_mutex_t *mutex, int *prioceiling) {
	return -ENOSYS;
}
*/

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
	mutex_init_default(mutex, attr);

	return ENOERR;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
	return mutex_lock(mutex);
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex,
				const struct timespec *abstime) {
	return mutex_timedlock(mutex, abstime);
}

/*
int pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int prioceiling, int *old_ceiling) {
	return -ENOSYS;
}
*/

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
	return mutex_trylock(mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	return mutex_unlock(mutex);
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
	return mutexattr_destroy(attr);
}

/*
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr, int *prioceiling) {
	return -ENOSYS;
}

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared) {
	return -ENOSYS;
}
*/

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) {
	return mutexattr_gettype(attr, type);
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
	return mutexattr_init(attr);
}

/*
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling) {
	return -ENOSYS;
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared) {
	return -ENOSYS;
}
*/

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
	return mutexattr_settype(attr, type);
}
