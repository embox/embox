/**
 * @file
 *
 * @date Sep 14, 2021
 * author Anton Bondarev
 */

#include <pthread.h>
#include <stddef.h>

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	return -ENOSYS;
}
