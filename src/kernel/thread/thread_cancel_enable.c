/**
 * @file
 *
 * @date Oct 15, 2013
 * @author: Anton Bondarev
 */
#include <pthread.h>
#include <assert.h>
#include <stddef.h>
#include <kernel/thread.h>
#include <kernel/sched.h>

#include <kernel/thread/thread_cancel.h>

int thread_cancel(struct thread *t) {
	sched_lock();
	{
		for (; t->cleanups.counter > 0; t->cleanups.counter--) {
			struct thread_cleanup *cleanup;

			cleanup = &t->cleanups.cleanups[t->cleanups.counter - 1];

			cleanup->routine(cleanup->arg);
		}

		thread_terminate(t);
	}
	sched_unlock();

	return ENOERR;
}

int thread_cancel_set_state(int state, int *oldstate) {
	struct thread *t;

	t = thread_self();

	if (oldstate != NULL) {
		*oldstate = t->cleanups.state;
	}

	return ENOERR;
}

int thread_cancel_set_type(int type, int *oldtype) {
	struct thread *t;

	t = thread_self();

	if (oldtype != NULL) {
		*oldtype = t->cleanups.type;
	}
	t->cleanups.type = type;

	return ENOERR;
}

int thread_cancel_cleanup_push(void (*routine)(void *), void *arg) {
	struct thread *t;
	struct thread_cleanup *cleanup;

	sched_lock();
	{
		t = thread_self();
		assert((CLEANUPS_QUANTITY > t->cleanups.counter) && (t->cleanups.counter >= 0));

		cleanup = &t->cleanups.cleanups[t->cleanups.counter++];

		cleanup->routine = routine;
		cleanup->arg = arg;
	}
	sched_unlock();

	return ENOERR;
}

int thread_cancel_cleanup_pop(int execute) {
	struct thread *t;
	struct thread_cleanup *cleanup;

	sched_lock();
	{
		t = thread_self();
		t->cleanups.counter--;
		assert(t->cleanups.counter >= 0);

		cleanup = &t->cleanups.cleanups[t->cleanups.counter];
	}
	sched_unlock();

	if (execute) {
		cleanup->routine(cleanup->arg);
	}

	return ENOERR;
}

int thread_cancel_init(struct thread *t) {
	t->cleanups.counter = 0;
	t->cleanups.state = PTHREAD_CANCEL_ENABLE;
	t->cleanups.type = PTHREAD_CANCEL_DEFERRED;

	return 0;
}
