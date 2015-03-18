/**
 * @file
 *
 * @date Oct 15, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <pthread.h>
#include <kernel/thread/thread_cancel.h>

int pthread_cancel(pthread_t thread) {
	return thread_cancel(thread);
}

void pthread_cleanup_push(void (*routine)(void *), void *arg) {
	thread_cancel_cleanup_push(routine, arg);
}

void pthread_cleanup_pop(int execute) {
	thread_cancel_cleanup_pop(execute);
}

int pthread_setcancelstate(int state, int *oldstate) {
	return thread_cancel_set_state(state, oldstate);
}

int pthread_setcanceltype(int type, int *oldtype) {
	return thread_cancel_set_type(type, oldtype);
}

void pthread_testcancel(void) {

}
