/**
 * @file
 *
 * @date Oct 15, 2013
 * @author: Anton Bondarev
 */
#include <pthread.h>
#include <assert.h>
#include <kernel/thread.h>

#include <kernel/thread/thread_cancel.h>

int thread_cancel(struct thread *t) {
	return -ENOSUPP;
}

int thread_cancel_set_state(int state, int *oldstate) {
	struct thread *t;

	t = thread_self();
	*oldstate = t->cleanups.state;

	return ENOERR;
}

int thread_cancel_set_type(int type, int *oldtype) {
	struct thread *t;

	t = thread_self();

	*oldtype = t->cleanups.type;
	t->cleanups.type = type;

	return ENOERR;
}

int thread_cancel_cleanup_push(void (*routine)(void *), void *arg) {
	struct thread *t;
	struct thread_cleanup *cleanup;

	t = thread_self();
	assert(CLEANUPS_QUANTITY < t->cleanups.counter);

	cleanup = &t->cleanups.cleanups[t->cleanups.counter];

	cleanup->routine = routine;
	cleanup->arg = arg;

	return ENOERR;
}

int thread_cancel_cleanup_pop(int execute) {
	struct thread *t;
	struct thread_cleanup *cleanup;

	t = thread_self();

	cleanup = &t->cleanups.cleanups[t->cleanups.counter];

	if(execute) {
		cleanup->routine(cleanup->arg);
	}

	t->cleanups.counter--;
	return -ENOSUPP;
}

int thread_cancel_init(struct thread *t) {
	t->cleanups.counter = 0;
	t->cleanups.state = PTHREAD_CANCEL_ENABLE;
	t->cleanups.type = PTHREAD_CANCEL_DEFERRED;

	return 0;
}
