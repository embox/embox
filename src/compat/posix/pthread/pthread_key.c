/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */

#include <pthread.h>
#include <kernel/thread.h>
#include <kernel/thread/thread_local.h>
#include <kernel/task/thread_key_table.h>

int pthread_setspecific(pthread_key_t key, const void *value) {
	pthread_t pt = pthread_self();

	return thread_local_set(pt, key, (void *)value);
}

void *pthread_getspecific(pthread_key_t key) {
	pthread_t pt = pthread_self();

	return thread_local_get(pt, key);
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
	pthread_t pt = pthread_self();

	return task_thread_key_create(pt->task, key);
}

int pthread_key_delete(pthread_key_t key) {
	pthread_t pt = pthread_self();

	return task_thread_key_destroy(pt->task, key);
}
