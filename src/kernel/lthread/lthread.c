/**
 * @file
 * @brief
 *
 * @author  Andrey Kokorev
 * @author  Vita Loginova
 * @date    21.11.2013
 */

#include <assert.h>
#include <errno.h>
#include <err.h>
#include <kernel/sched.h>
#include <kernel/lthread/lthread.h>
#include <kernel/runnable/runnable.h>
#include <kernel/lthread/lthread_priority.h>
#include <mem/misc/pool.h>

/**
 * Memory have to be allocated only for lthread structure, while lthread uses
 * thread stack when executed
 */
#define POOL_SZ       OPTION_GET(NUMBER, lthread_pool_size)

POOL_DEF(lwthread_pool, struct lthread, POOL_SZ);
/*
 * Called in __schedule
*/
void lthread_trampoline(struct runnable *r) {
	struct lthread *lt;

	r->run(r->run_arg);
	lt = mcast_out(r, struct lthread, runnable);
	pool_free(&lwthread_pool, lt);
}

static void lwthread_init(struct lthread *lt, void *(*run)(void *), void *arg) {
	assert(lt);

	lt->runnable.run = run;
	lt->runnable.prepare = NULL;
	lt->runnable.run_arg = arg;

	runq_item_init(&lt->runnable.sched_attr.runq_link);
	sched_affinity_init(&lt->runnable);
	runnable_priority_init(&lt->runnable, LWTHREAD_PRIORITY_DEFAULT);
}

struct lthread *lthread_create(void *(*run)(void *), void *arg) {
	struct lthread *lt;

	if (!run) {
		return err_ptr(EINVAL);
	}

	if (!(lt = (struct lthread *) pool_alloc(&lwthread_pool))) {
		return err_ptr(ENOMEM);
	}

	lwthread_init(lt, run, arg);

	return lt;
}

void lthread_launch(struct lthread *lt) {
	assert(lt);
	sched_wakeup_l(lt);
}
