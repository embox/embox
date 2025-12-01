/**
 * @file
 *
 * @date Oct 15, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <pthread.h>


int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol) {
	return -ENOSYS;
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol) {
	return -ENOSYS;
}
