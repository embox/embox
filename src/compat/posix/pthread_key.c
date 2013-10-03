/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */

#include <pthread.h>


int pthread_setspecific(pthread_key_t key, const void *value) {
	return -ENOSYS;
}

void *pthread_getspecific(pthread_key_t key) {
	return NULL;
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
	return -ENOSYS;
}

int pthread_key_delete(pthread_key_t key) {
	return -ENOSYS;
}
