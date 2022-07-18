/**
 * @file
 *
 * @date Jul 18, 2022
 * @author Anton Bondarev
 */

#include <errno.h>
#include <pthread.h>

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr) {
	return -ENOSYS;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	return -ENOSYS;
}
