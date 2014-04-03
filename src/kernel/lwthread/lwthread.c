/*

*/
#include <assert.h>
#include <errno.h>
#include <err.h>
#include <kernel/sched.h>
#include <kernel/lwthread/lwthread.h>
#include <kernel/runnable/runnable.h>
#include <kernel/lwthread/lwthread_priority.h>
#include <mem/misc/pool.h>

typedef struct lwthread lwthread_pool_entry_t;

/**
 * Memory have to be allocated only for lwthread structure,
 * while lwthread uses thread stack when executed
*/
#define POOL_SZ       OPTION_GET(NUMBER, lwthread_pool_size)

POOL_DEF(lwthread_pool, lwthread_pool_entry_t, POOL_SZ);

/*
 * Called in __schedule
*/
void lwthread_trampoline(struct runnable *r) {
	struct lwthread *lwt;
	lwt = mcast_out(r, struct lwthread, runnable);

	lwt->runnable.run(lwt->runnable.run_arg);

	lwthread_free(lwt);
}

void lwthread_init(struct lwthread *lwt, void *(*run)(void *), void *arg) {
	assert(lwt);

	lwt->runnable.run = (void *)run;
	lwt->runnable.prepare = NULL;
	lwt->runnable.run_arg = arg;

	sched_lock();
	{
		runq_item_init(&(lwt->runnable.sched_attr.runq_link));
		sched_affinity_init(&(lwt->runnable));
		lwthread_priority_init(lwt, LWTHREAD_PRIORITY_DEFAULT);
	}
	sched_unlock();
}

struct lwthread * lwthread_create(void *(*run)(void *), void *arg) {
	struct lwthread *lwt;
	lwthread_pool_entry_t *block;

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

	assert(lwt);

	block = (lwthread_pool_entry_t *) lwt;
	lwt = (struct lwthread *)block;
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
