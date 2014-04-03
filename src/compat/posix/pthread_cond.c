/**
 * @file
 *
 * @date Oct 15, 2013
 * @author: Anton Bondarev
 */

#include <pthread.h>
#include <errno.h>

int pthread_cond_broadcast(pthread_cond_t *cond) {
	return cond_broadcast(cond);
}

int pthread_cond_destroy(pthread_cond_t *cond) {
	cond_destroy(cond);

	return ENOERR;
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
	cond_init(cond, attr);

	return ENOERR;
}

int pthread_cond_signal(pthread_cond_t *cond) {
	return cond_signal(cond);
}


int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
		const struct timespec *abstime) {
	return cond_timedwait(cond, mutex, abstime);
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	return cond_wait(cond, mutex);
}

int pthread_condattr_init(pthread_condattr_t *attr) {
	condattr_init(attr);

	return ENOERR;
}


int pthread_condattr_destroy(pthread_condattr_t *attr) {
	condattr_destroy(attr);

	return ENOERR;
}

int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared) {
	condattr_getpshared(attr, pshared);

	return ENOERR;
}


int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared) {
	condattr_setpshared(attr, pshared);

	return ENOERR;
}

int pthread_condattr_getclock(const pthread_condattr_t *restrict attr,
		clockid_t *restrict clock_id) {
	condattr_getclock(attr, clock_id);

	return ENOERR;

}
int pthread_condattr_setclock(pthread_condattr_t *attr,
		clockid_t clock_id) {
	condattr_setclock(attr, clock_id);

	return ENOERR;
}
