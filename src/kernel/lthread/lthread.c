/**
 * @file
 * @brief
 *
 * @author  Andrey Kokorev
 * @author  Vita Loginova
 * @date    21.11.2013
 */

#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <err.h>
#include <kernel/sched.h>
#include <kernel/lthread/lthread.h>
#include <kernel/sched/schedee.h>
#include <kernel/sched/current.h>
#include <kernel/lthread/lthread_priority.h>
#include <mem/misc/pool.h>

/**
 * Memory have to be allocated only for lthread structure, while lthread uses
 * thread stack when executed
 */
#define LTHREAD_POOL_SIZE OPTION_GET(NUMBER, lthread_pool_size)

POOL_DEF(lthread_pool, struct lthread, LTHREAD_POOL_SIZE);

/* locks: sched. lthread->run must be atomic. */
static int lthread_process(struct schedee *prev, struct schedee *next,
		ipl_t ipl) {
	struct lthread *lt = mcast_out(next, struct lthread,
			schedee);
	schedee_set_current(next);

	/* lthread is not in runq, it can be waken up again. */
	next->ready = false;

	/* We have to restore ipl as soon as possible. */
	ipl_restore(ipl);

	lt->run_ret = next->run(next->run_arg);

	next->waiting = true;

	return SCHEDEE_REPEAT;
}

static void lthread_init(struct lthread *lt, void *(*run)(void *), void *arg) {
	assert(lt);

	lt->schedee.run = run;
	lt->schedee.process = lthread_process;
	lt->schedee.run_arg = arg;

	lt->schedee.ready = false;
	lt->schedee.active = false;
	lt->schedee.waiting = true;

	lt->schedee.lock = SPIN_UNLOCKED;

	schedee_init(&lt->schedee);
	lthread_priority_init(lt, LTHREAD_PRIORITY_DEFAULT);
	sched_wait_info_init(&lt->info);
}

struct lthread *lthread_create(void *(*run)(void *), void *arg) {
	struct lthread *lt;

	if (!run) {
		return err_ptr(EINVAL);
	}

	if (!(lt = (struct lthread *) pool_alloc(&lthread_pool))) {
		return err_ptr(ENOMEM);
	}

	lthread_init(lt, run, arg);

	return lt;
}

void lthread_delete(struct lthread *lt) {
	assert(lt);
	pool_free(&lthread_pool, lt);
}

void lthread_launch(struct lthread *lt) {
	assert(lt);
	sched_wakeup(&lt->schedee);
}
