/**
 * @file
 *
 * @date Oct 15, 2013
 * @author: Anton Bondarev
 */

#include <pthread.h>

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr) {
	return -ENOSYS;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr) {
	return -ENOSYS;
}

int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *rwlock, int *pshared) {
	return -ENOSYS;
}

int pthread_rwlockattr_init(pthread_rwlockattr_t *rwlock) {
	return -ENOSYS;
}

int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *rwlock, int pshared) {
	return -ENOSYS;
}
