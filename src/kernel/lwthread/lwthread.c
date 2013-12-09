/*

*/
#include <errno.h>
#include <kernel/lwthread.h>
#include <mem/misc/pool.h>

typedef union lwthread_pool_entry {
	struct lwthread lwthread;
} lwthread_pool_entry_t;

/**
 * Memory have to be allocated only for lwthread structure,
 * while execute lwthread uses thread stack
*/
#define POOL_SZ sizeof(lwthread_pool_entry_t);

POOL_DEF(lwthread_pool, lwthread_pool_entry_t, POOL_SZ);


void lthread_init(struct lwthread *lwt, void *(*run)(void *), void *arg) {
	lt->runnable->run = run;
	lt->runnable->prepare = NULL;

	lt->run_arg = arg;
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

	lwt = &(block->lwthread);

	lwthread_init(lwt, run, arg);

	return lwt;
}

void lwthread_free(struct lwthread *lwt) {
	lwthread_pool_entry_t *block;

	assert(lwt != NULL);

	block = member_cast_out(lwt, lwthread_pool_entry_t, lwthread);
	pool_free(&lwthread_pool, block);
}
