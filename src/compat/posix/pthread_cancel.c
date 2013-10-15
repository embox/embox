/**
 * @file
 *
 * @date Oct 15, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <pthread.h>

int pthread_cancel(pthread_t thread) {
	return -ENOSYS;
}

void pthread_cleanup_push(void (*routine)(void *), void *arg) {

}

void pthread_cleanup_pop(int execute) {

}

int pthread_setcancelstate(int state, int *oldstate) {
	return -ENOSYS;
}

int pthread_setcanceltype(int type, int *oldtype) {
	return -ENOSYS;
}

/*

void pthread_testcancel(void) {

}
*/
