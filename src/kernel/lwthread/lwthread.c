/*

*/
#include <assert.h>
#include <errno.h>
#include <err.h>
#include <kernel/sched.h>
#include <kernel/lwthread.h>
#include <mem/misc/pool.h>

typedef struct lwthread lwthread_pool_entry_t;

/**
 * Memory have to be allocated only for lwthread structure,
 * while execute lwthread uses thread stack
*/
#define POOL_SZ       OPTION_GET(NUMBER, lwthread_pool_size)

POOL_DEF(lwthread_pool, lwthread_pool_entry_t, POOL_SZ);

void lwthread_init(struct lwthread *lwt, void *(*run)(void *), void *arg) {
	lwt->runnable.run = (void *)run;
	lwt->runnable.prepare = NULL;

	lwt->runnable.run_arg = arg;
}

struct lwthread * lwthread_create(void *(*run)(void *), void *arg) {
	struct lwthread *lwt;
	lwthread_pool_entry_t *block;

	/* check correct executive function */
	if (!run) {
		return err_ptr(EINVAL);
	}

	if (!(block = (lwthread_pool_entry_t *) pool_alloc(&lwthread_pool))) {
		return err_ptr(ENOMEM);
	}

	lwt = (struct lwthread *) block;

	lwthread_init(lwt, run, arg);

	return lwt;
}

void lwthread_free(struct lwthread *lwt) {
	lwthread_pool_entry_t *block;

	assert(lwt != NULL);

	block = (lwthread_pool_entry_t *) lwt;
	pool_free(&lwthread_pool, block);
}

void lwthread_launch(struct lwthread *lwt) {
	assert(lwt);

	sched_lock();
	{
		sched_lwthread_wake(lwt);
	}
	sched_unlock();
}
